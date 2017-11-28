#include "objectreserver.h"
#include "dataacquisition.h"
#include "detectorfilename.h"
#include "detectorfactory.h"
#include "voltageSourceFactory.h"
//#include "processingwindow.h"
#include "math.h"
//#include "motor.h"

#include <QDebug>
#include <QMessageBox>
#include <QDateTime>

DataAcquisition *DataAcquisition::daqInstance = 0;
//double DataAcquisition::motorPosition = 0;
//QHash<QString, int> DataAcquisition::motorPositions;

DataAcquisition::DataAcquisition(QObject *parent) :
   QThread(parent)
{
   collecting = false;
   triggered = false;
   biasRefreshing = false;
   monitoring = false;
   configuring = false;
   biasOn = false;
   biasRefreshRequired = false;
   setAbort(false);
   gigEDetector = DetectorFactory::instance()->getGigEDetector();
   detectorState = GigEDetector::IDLE;
   hv = VoltageSourceFactory::instance()->getHV();
   biasPriority = hv->getBiasPriority();
   tdp = 0.0;
   currentImageNumber = 0;
   daqStatus = DataAcquisitionStatus();
   qRegisterMetaType<DataAcquisitionStatus>("DataAcquisitionStatus");
   qRegisterMetaType<DataAcquisitionDefinition>("DataAcquisitionDefinition");
   qRegisterMetaType<GigEDetector::DetectorCommand>("GigEDetector::DetectorCommand");
   qRegisterMetaType<GigEDetector::DetectorState>("GigEDetector::DetectorState");
   busy = false;
   rdaql.append(this);
}

//void DataAcquisition::positionChanged(Motor *motor, const QVariant & value)
//{
//   int position = value.toInt();
//   QString name = motor->property("objectName").toString();
//   motorPositions[name] = position;
//}

void DataAcquisition::configureTriggering(int triggeringMode)
{
   mode = GigEDetector::RECONFIGURE;
   gigEDetector->setTriggeringMode(triggeringMode);
}

void DataAcquisition::configureBasicCollection()

{
   dataAcquisitionModel = DataAcquisitionModel::getInstance();
   dataAcquisitionDefinition = dataAcquisitionModel->getDataAcquisitionDefinition();
   gigEDetector->setTimestampOn(false);
   gigEDetector->setDataAcquisitionDuration(1000);
   mode = GigEDetector::GIGE_DEFAULT;
   gigEDetector->setMode(mode);
}

void DataAcquisition::configureDataCollection()
{
   dataAcquisitionModel = DataAcquisitionModel::getInstance();
   QList<QObject *> rdaqml = dataAcquisitionModel->getReserveList();


   Reservation modelReservation = ObjectReserver::instance()->reserveForGUI(rdaqml);
   reservation = reservation.add(modelReservation);

   if (reservation.getReserved().isEmpty())
   {
      qDebug() <<"configureDataCollection Could not reserve all objects, message = " << reservation.getMessage();
   }
   else
   {
      dataAcquisitionDefinition = dataAcquisitionModel->getDataAcquisitionDefinition();
      gigEDetector->setTimestampOn(dataAcquisitionDefinition->getDataFilename()->getTimestampOn());
      gigEDetector->setDirectory(dataAcquisitionDefinition->getDataFilename()->getDirectory());
      gigEDetector->setPrefix(dataAcquisitionDefinition->getDataFilename()->getPrefix());
      gigEDetector->setDataAcquisitionDuration(dataAcquisitionDefinition->getDuration());
      if (dataAcquisitionDefinition->getOffsets())
      {
         gigEDetector->enableDarks();
      }
      else
      {
         gigEDetector->disableDarks();
      }

      setAbort(false);
      nRepeat = 1;
      appendRepeatCount = false;


      if (biasOn && biasPriority)
      {
         splitDataCollections = ceil(((double) dataAcquisitionDefinition->getDuration()) / ((double )hv->getBiasRefreshInterval()));
      }
      else
      {
         splitDataCollections = 1;
      }

      if ((nRepeat = dataAcquisitionDefinition->getRepeatCount()) > 1)
      {
         appendRepeatCount = true;
      }

      mode = GigEDetector::CONTINUOUS;
      gigEDetector->setMode(mode);
      currentImageNumber = 0;

      daqStatus.setDaqImages(splitDataCollections * nRepeat);
      daqStatus.setCurrentImage(0);
      daqStatus.setPercentage(0);
      changeDAQStatus(DataAcquisitionStatus::ACQUIRING_DATA,
                      daqStatus.getMinorStatus());
   }
}

DataAcquisition::~DataAcquisition()
{
   hv->off();
   emit executeCommand(GigEDetector::KILL, 0, 0);
   /* Stops crash on application exit.
      Increased following registered callback use.
      Increased following introduction of DAQ status and crashing post triggered DAQ. */
   Sleep(3000);
}

bool DataAcquisition::isBusy()
{
   return busy;
}

char *DataAcquisition::getStatus()
{
   int major = daqStatus.getMajorStatus();
   int minor = daqStatus.getMinorStatus();
   char *status;
   sprintf(status,"%d.%d", major, minor);
   return status;//daqStatus.getMajorStatus() + "." + daqStatus.getMinorStatus();
}

void DataAcquisition::run()
{
   try
   {
   if (mode == GigEDetector::GIGE_DEFAULT)
   {
      performGigEDefaultDataCollection();
      // TODO : would emiting this to DataAcquisition be better for thread safety
      changeDAQStatus(DataAcquisitionStatus::IDLE,
                      daqStatus.getMinorStatus());
   }
   else if (mode == GigEDetector::CONTINUOUS)
   {
      if (dataAcquisitionDefinition->isTriggering())
      {
         performContinuousDataCollection(true);
      }
      else
      {
         performContinuousDataCollection();
      }
      // TODO : would emiting this to DataAcquisition be better for thread safety
      changeDAQStatus(daqStatus.getMajorStatus(),
                      DataAcquisitionStatus::DONE);
      changeDAQStatus(DataAcquisitionStatus::IDLE,
                      DataAcquisitionStatus::READY);
   }
   else if (mode == GigEDetector::RECONFIGURE)
   {
      performTriggeringConfigure();
   }
/*   else if (mode == GigEDetector::FIXED)
   {
      performFixedDataCollection();
      // TODO : would emiting this to DataAcquisition be better for thread safety
      changeDAQStatus(DataAcquisitionStatus::IDLE,
                      daqStatus.getMinorStatus());
   }
   */
   }
   catch (DetectorException &ex)
   {
//         emit writeError(ex.getMessage());
      qDebug() << "DetectorException caught: ";
      qDebug() << "Message: " << ex.getMessage();
   }
}

void DataAcquisition::setDirectory(int repeatCount)
{
   QString *dir = new QString(dataAcquisitionDefinition->getDataFilename()->getDirectory());

   if (appendRepeatCount)
   {
      dir->append("/" + dataAcquisitionDefinition->getDataFilename()->getPrefix() + QString().sprintf("repeat_%03d", repeatCount));
   }

   gigEDetector->setDirectory(*dir);
   delete dir;
}

void DataAcquisition::performTriggeringConfigure()
{
   configuring = true;
   emit storeBiasSettings();
   emit disableBiasRefresh();
   emit disableMonitoring();

   emit executeCommand(GigEDetector::CONFIGURE, 0, 0);
   waitForConfiguringDone();

   emit restoreBiasSettings();
   emit enableMonitoring();

}

void DataAcquisition::performContinuousDataCollection(bool triggering)
{
   int nDaq;
   int repeatCount;
   int nDaqOverall = 0;

   emit storeBiasSettings();
   emit disableBiasRefresh();
   emit disableMonitoring();
   dataAcquisitionModel = DataAcquisitionModel::getInstance();

   for (repeatCount = 0; repeatCount < nRepeat; repeatCount++)
   {
      totalFramesAcquired = 0;
      emit appendTimestamp(true);
      setDirectory(repeatCount);
      emit imageStarting(dataAcquisitionModel->getDaqCollectionDuration()/1000, repeatCount, nRepeat);
      performMonitorEnvironmentalValues();

      if (biasPriority)
      {
         nDaqOverall = doSplitDataCollections(nDaqOverall, repeatCount, triggering);
      }
      else
      {
         nDaqOverall = doLowPriorityBiasDataCollections(nDaqOverall);
      }

      emit imageComplete(totalFramesAcquired);
      emit imageComplete((long long)totalFramesAcquired);

      // Break the outer loop too.
      if (abortRequired())
         break;

      if (repeatPauseRequired(repeatCount))
      {
         pauseDataAcquisition();
         
         if (abortRequired())
            break;
         changeDAQStatus(daqStatus.getMajorStatus(),
                         DataAcquisitionStatus::COLLECTING);
      }
   }

   daqStatus.setCurrentImage(++nDaqOverall);
   emit dataAcquisitionStatusChanged(daqStatus);

   gigEDetector->setDataAcquisitionDuration(dataAcquisitionDefinition->getDuration());
   emit restoreBiasSettings();
   emit enableMonitoring();
}

int DataAcquisition::doSplitDataCollections(int nDaqOverall, int repeatCount, bool triggering, int ttlInput)
{
   bool suspendTriggering = false;
   bool configureRequired = false;

   for (nDaq = 0; nDaq < splitDataCollections ; nDaq++)
   {
      setDataAcquisitionTime(nDaq);
      triggered = false;

      if (nDaq > 0)
      {
         emit appendTimestamp(false);
         collecting = true;
         emit executeCommand(GigEDetector::COLLECT,
                             dataAcquisitionDefinition->getRepeatCount(), nDaqOverall);
      }
      else
      {
         collecting = true;
         emit executeCommand(GigEDetector::COLLECT,
                             dataAcquisitionDefinition->getRepeatCount(), nDaqOverall);
         if (triggering && (splitDataCollections > 1))
         {
            suspendTriggering = true;
         }
      }

      nDaqOverall++;
      waitForCollectingDone();

      daqStatus.setCurrentImage(nDaqOverall);
      if (abortRequired())
      {
         break;
      }

      if (nDaq < (splitDataCollections - 1) ||
          !repeatPauseRequired(repeatCount))
      {
         performMonitorEnvironmentalValues();
         performSingleBiasRefresh();
         if (abortRequired())
         {
            break;
         }
      }
      if (suspendTriggering)
      {
         configuring = true;
         emit executeCommand(GigEDetector::CONFIGURE, 1, 0);
         waitForConfiguringDone();
         configureRequired = true;
         suspendTriggering = false;
      }
   }

   if (configureRequired && (!abortRequired()))
   {
      configuring = true;
      emit executeCommand(GigEDetector::CONFIGURE, 0, 0);
      waitForConfiguringDone();
   }

   return nDaqOverall;
}

int DataAcquisition::doLowPriorityBiasDataCollections(int nDaqOverall)
{
   unsigned long long remainingFrames = -1;
   totalImageFrames = -1;
   bool startOfImage = true;

   dataCollectionTime = dataAcquisitionDefinition->getDuration();
   collecting = true;
   nDaqOverall++;
   emit appendTimestamp(true);

   remainingFrames = gigEDetector->getRemainingFrames();
   totalImageFrames = remainingFrames;

   while (remainingFrames != 0)
   {
      if (!startOfImage)
      {
         emit appendTimestamp(false);
      }
      hv->setReadyForRefresh(false);
      collecting = true;
      emit executeCommand(GigEDetector::RESTART, startOfImage, 0);
      waitForCollectingDone();
      if (abortRequired())
      {
         break;
      }
      remainingFrames = gigEDetector->getRemainingFrames();

      if (remainingFrames != 0)
      {
         performMonitorEnvironmentalValues();
         biasRefreshing = true;  // NEWLY ADDED
         emit executeSingleBiasRefresh();
         waitForBiasRefreshDone();
      }
      startOfImage = false;
      sleep(0.1);
   }

   daqStatus.setCurrentImage(nDaqOverall);

   return nDaqOverall;
}

void DataAcquisition::performGigEDefaultDataCollection()
{
   dataAcquisitionModel = DataAcquisitionModel::getInstance();
   dataAcquisitionDefinition = dataAcquisitionModel->getDataAcquisitionDefinition();

   emit storeBiasSettings();
   emit disableBiasRefresh();
   emit disableMonitoring();
//   waitForMonitoringDone();

   collecting = true;
   emit executeCommand(GigEDetector::COLLECT, dataAcquisitionDefinition->getFixedImageCount(), 1);
   waitForCollectingDone();
   collecting = false;
   emit restoreBiasSettings();
   emit enableMonitoring();
}

bool DataAcquisition::repeatPauseRequired(int repeatCount)
{
   bool repeatPauseRequiredFlag = false;

   if ((repeatCount < nRepeat - 1) &&
       (dataAcquisitionDefinition->getRepeatInterval() > 0))
   {
      repeatPauseRequiredFlag = true;
   }

   return repeatPauseRequiredFlag;
}

void DataAcquisition::performFixedDataCollection()
{
   dataAcquisitionModel = DataAcquisitionModel::getInstance();
   dataAcquisitionDefinition = dataAcquisitionModel->getDataAcquisitionDefinition();
   emit storeBiasSettings();
   emit disableBiasRefresh();

   collecting = true;
   emit executeCommand(GigEDetector::COLLECT, dataAcquisitionDefinition->getFixedImageCount(), 1);

   waitForCollectingDone();
   collecting = false;

   emit restoreBiasSettings();
}

void DataAcquisition::setDataAcquisitionTime(int nDaq)
{
   double biasRefreshDataCollectionTime = hv->getBiasRefreshInterval();
   double finalDataCollectionTime = dataAcquisitionDefinition->getDuration() - (biasRefreshDataCollectionTime * ((double) (splitDataCollections - 1)));
   double durationSeconds;

   dataCollectionTime = biasRefreshDataCollectionTime;

   if (nDaq == (splitDataCollections - 1))
   {
      dataCollectionTime = finalDataCollectionTime;
   }

   if (nDaq != 0)
   {
//      dataCollectionTime -= gigEDetector->getTimeError();
   }

   totalImageFrames = gigEDetector->setDataAcquisitionDuration(dataCollectionTime);
}

void DataAcquisition::performMonitorEnvironmentalValues()
{
   monitoring = true;
   emit executeMonitorEnvironmentalValues();
   waitForMonitoringDone();
}

void DataAcquisition::waitForMonitoringDone()
{
   while (monitoring)
      sleep(0.1);
}

void DataAcquisition::performSingleBiasRefresh()
{
   if (biasOn)
   {
      changeDAQStatus(daqStatus.getMajorStatus(), DataAcquisitionStatus::BIAS_REFRESHING);
      biasRefreshing = true;
      emit executeSingleBiasRefresh();
      waitForBiasRefreshDone();
      emit disableBiasRefresh();
   }
}

void DataAcquisition::pauseDataAcquisition()
{
   int pauseDuration = 0;

   if ((pauseDuration = dataAcquisitionDefinition->getRepeatInterval()) > 0)
   {
      changeDAQStatus(daqStatus.getMajorStatus(), DataAcquisitionStatus::PAUSED);
      for (int i = 0; i < pauseDuration; i+=10)
      {
         Sleep(10);

         if (abortRequired())
         {
            break;
         }
      }
      performSingleBiasRefresh();
   }
}

DataAcquisition *DataAcquisition::instance()
{
   if (daqInstance == 0)
   {
      daqInstance = new DataAcquisition();
   }
   return daqInstance;
}

int DataAcquisition::waitForBiasRefreshDone()
{
   int status = 0;

   changeDAQStatus(daqStatus.getMajorStatus(), DataAcquisitionStatus::BIAS_REFRESHING);
   while (biasRefreshing)
   {
      sleep(0.1);
   }

   return status;
}

int DataAcquisition::waitForTrigger()
{
   int status = 0;

   while (!triggered)
      sleep(0.1);

   return status;

}

int DataAcquisition::waitForConfiguringDone()
{
   int status = 0;

   while (configuring)
      sleep(0.1);

   return status;
}

int DataAcquisition::waitForCollectingDone()
{
   int status = 0;
   int elapsed = 0;
   int percentage = 0;
   unsigned long long remainingFrames;

   while (collecting)
   {
      sleep(1);
      if (mode == GigEDetector::CONTINUOUS)
      {
         elapsed++;
         if (daqStatus.getMinorStatus() == DataAcquisitionStatus::OFFSETS)
         {
            percentage = (100000.0 * (double) elapsed / dataCollectionTime) + 0.5;
         }
         else
         {
            remainingFrames = gigEDetector->getRemainingFrames();
            percentage = ((double)totalImageFrames - (double)remainingFrames) / (double)totalImageFrames * 100.0;
            if (percentage > 0)
            {
               changeDAQStatus(daqStatus.getMajorStatus(), DataAcquisitionStatus::COLLECTING);
            }
         }

         if (percentage > 100)
         {
            percentage = 100;
         }
         daqStatus.setPercentage(percentage);
         emit dataAcquisitionStatusChanged(daqStatus);
      }
   }

   if (mode == GigEDetector::GIGE_DEFAULT)
   {
      percentage = 100;
      daqStatus.setPercentage(percentage);
      emit dataAcquisitionStatusChanged(daqStatus);
   }
   else if ((remainingFrames = gigEDetector->getRemainingFrames()) != 0)
   {
      percentage = ((double)totalImageFrames - (double)remainingFrames) / (double)totalImageFrames * 100.0;
   }
   else
   {
      percentage = 100;
   }
   return status;
}

void DataAcquisition::handleSetFingerTemperature(double temperature)
{
   if (temperature < tdp)
   {
      QMessageBox msgBox;
      msgBox.setText(QString("Cannot set a Finger Temperature lower than dew point\n") +
                     QString("Dew point currently " + QString::number(tdp, 'f', 1)));
      msgBox.setIcon(QMessageBox::Warning);
      msgBox.exec();
   }
   else
   {
     emit setTargetTemperature(temperature);
   }
}

void DataAcquisition::handleMonitorData(MonitorData *md)
{
   if (md->getValid())
   {
      tdp = md->getTDP();
   }
   else
   {
      qDebug() << "Monitoring Failed!";
   }

}

void DataAcquisition::changeDAQStatus(DataAcquisitionStatus::MajorStatus majorStatus,
                                      DataAcquisitionStatus::MinorStatus minorStatus)
{
   if ((majorStatus == DataAcquisitionStatus::IDLE) && (minorStatus == DataAcquisitionStatus::READY))
   {
      busy = false;
      ObjectReserver::instance()->release(reservation.getReserved(), "GUIReserver");
   }
   daqStatus.setMajorStatus(majorStatus);
   daqStatus.setMinorStatus(minorStatus);
   emit dataAcquisitionStatusChanged(daqStatus);
}

void DataAcquisition::receiveState(GigEDetector::DetectorState detectorState)
{
   this->detectorState = detectorState;
   busy = true;

   switch (detectorState)
   {
   case GigEDetector::IDLE:
         changeDAQStatus(daqStatus.getMajorStatus(),
                         DataAcquisitionStatus::NOT_INITIALIZED);
      break;
   case GigEDetector::READY:
      if (daqStatus.getMajorStatus() == DataAcquisitionStatus::INITIALISING)
      {
         daqStatus.setMajorStatus(DataAcquisitionStatus::IDLE);
      }
      changeDAQStatus(daqStatus.getMajorStatus(),
                      DataAcquisitionStatus::READY);
      if (collecting)
      {
         collecting = false;
      }
      if (configuring)
      {
         configuring = false;
      }
      break;
   case GigEDetector::INITIALISING:
      changeDAQStatus(DataAcquisitionStatus::INITIALISING,
                      DataAcquisitionStatus::ACTIVE);
      break;
   case GigEDetector::INITIALISED:
      changeDAQStatus(DataAcquisitionStatus::INITIALISING,
                      DataAcquisitionStatus::DONE);
      emit enableMonitoring();
      emit enableBiasRefresh();
      break;
   case GigEDetector::WAITING_DARK:
      changeDAQStatus(daqStatus.getMajorStatus(),
                      DataAcquisitionStatus::WAITING_DARK);
      break;
   case GigEDetector::OFFSETS:
      changeDAQStatus(daqStatus.getMajorStatus(),
                      DataAcquisitionStatus::OFFSETS);
      break;
   case GigEDetector::OFFSETS_PREP:
      changeDAQStatus(daqStatus.getMajorStatus(),
                      DataAcquisitionStatus::OFFSETS_PREP);
      break;
   case GigEDetector::COLLECTING_PREP:
      changeDAQStatus(daqStatus.getMajorStatus(),
                      DataAcquisitionStatus::COLLECTING_PREP);
      break;
   case GigEDetector::COLLECTING:
      changeDAQStatus(DataAcquisitionStatus::ACQUIRING_DATA,
                      DataAcquisitionStatus::COLLECTING);
      break;
   case GigEDetector::WAITING_TRIGGER:
      changeDAQStatus(DataAcquisitionStatus::ACQUIRING_DATA,
                      DataAcquisitionStatus::WAITING_TRIGGER);
         break;
   }
}

void DataAcquisition::handleTriggeringSelectionChanged(int triggeringMode)
{
   reservation = ObjectReserver::instance()->reserveForGUI(rdaql);
   if (reservation.getReserved().isEmpty())
   {
      qDebug() << "handleintTriggeringSelectionChanged Could not reserve all objects, message = " << reservation.getMessage();
   }
   else
   {
      qDebug() << "handleTriggeringSelectionChanged all objects reserved, configuring triggering";
      configureTriggering(triggeringMode);
//      try
//      {
         start();
//      }
//      catch (DetectorException &ex)
//      {
//         emit writeError(ex.getMessage());
//         qDebug() << "DetectorException caught: " << ex.getMessage();
//      }

   }
}

void DataAcquisition::handleTtlInputSelectionChanged(int ttlInput)
{
   gigEDetector->setTtlInput(ttlInput);
}

void DataAcquisition::collectReducedImages()
{
   handleCollectReducedImages();
}

void DataAcquisition::handleCollectReducedImages()
{
   reservation = ObjectReserver::instance()->reserveForGUI(rdaql);
   if (reservation.getReserved().isEmpty())
   {
      qDebug() << "handleCollectReducedImages Could not reserve all objects, message = " << reservation.getMessage();
   }
   else
   {
      qDebug() << "handleCollectReducedImages all objects reserved, configuring DAQ";
      configureDataCollection();

      start();
   }
}

void DataAcquisition::handleCollectFixedImages()
{
   configureBasicCollection();
   start();
}

void DataAcquisition::handleBufferReady(unsigned char *transferBuffer, unsigned long validFrames)
{
   if (mode != GigEDetector::GIGE_DEFAULT)
   {
      emit transferBufferReady(transferBuffer, validFrames);
      triggered = true;

   }
   ///  HexitecGigE Addition:
   /// Must provide a set of motorPositions also,
   /// or bufferQueue fills up while motorQueue remains empty..
}

/*
void DataAcquisition::handleImageStarted(char *path, int frameSize)
{
   qDebug() << "DataAcquisition::handleImageStarted(), path = " << path;
//   hxtProcessor->pushRawFileName(path, frameSize);
//   hxtProcessor->pushMotorPositions(&motorPositions); /// Provide motorPositions together with buffer instead
}
*/
void DataAcquisition::handleImageComplete(unsigned long long framesAcquired)
{
   totalFramesAcquired += framesAcquired;
}

void DataAcquisition::handleInitialiseDetector()
{
   gigEDetector->initialiseConnection();
   emit enableMonitoring();
   biasOn = false;
}

void DataAcquisition::prepareForBiasRefresh()
{
   gigEDetector->abort(true);
   hv->setReadyForRefresh(true);
}

void DataAcquisition::handleExecuteOffsets()
{
   emit executeOffsets();
}

void DataAcquisition::handleCancelOffsets()
{
   abort = true;
   collecting = false;
   emit executeCommand(GigEDetector::STATE, GigEDetector::READY, 0);
   // Following 2 line necessary for when soft trigger cancelled.
   gigEDetector->setDataAcquisitionDuration(dataAcquisitionDefinition->getDuration());
   emit restoreBiasSettings();
}

void DataAcquisition::handleExecuteReducedDataCollection()
{
   emit executeReducedDataCollection();
}

void DataAcquisition::handleCancelReducedDataCollection()
{
   abort = true;
   collecting = false;
   emit executeCommand(GigEDetector::STATE, GigEDetector::READY, 0);
   // Following 2 line necessary for when soft trigger cancelled.
   gigEDetector->setDataAcquisitionDuration(dataAcquisitionDefinition->getDuration());
   emit restoreBiasSettings();
}

void DataAcquisition::handleBiasRefreshing()
{
   storedMinorStatus = daqStatus.getMinorStatus();
   changeDAQStatus(daqStatus.getMajorStatus(),
                   DataAcquisitionStatus::BIAS_REFRESHING);
   biasRefreshing = true;
}

void DataAcquisition::handleBiasRefreshed(QString time, bool restartMonitoring)
{
   changeDAQStatus(daqStatus.getMajorStatus(),
                   storedMinorStatus);
   biasRefreshing = false;
   if (restartMonitoring)
   {
      emit enableMonitoring();
   }
}

void DataAcquisition::handleMonitored()
{
   monitoring = false;
}

void DataAcquisition::handleBiasState(bool biasOn)
{
   this->biasOn= biasOn;
}

void DataAcquisition::handleAbortDAQ()
{
//   setAbort(true);
//   gigEDetector->abort(true);
   setAbort(true);
   gigEDetector->abort(false);
}

void DataAcquisition::setAbort(bool abort)
{
   this->abort = abort;
}

bool DataAcquisition::abortRequired()
{
   return abort;
}
