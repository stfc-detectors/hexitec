#include "objectreserver.h"
#include "dataacquisition.h"
#include "detectorfilename.h"
#include "detectorfactory.h"
#include "voltageSourceFactory.h"
#include "processingwindow.h"
#include "math.h"
#include "motor.h"

#include <QDebug>
#include <QMessageBox>

DataAcquisition *DataAcquisition::daqInstance = 0;
hexitech::HxtProcessing *DataAcquisition::hxtProcessor = 0;
double DataAcquisition::motorPosition = 0;
QHash<QString, int> DataAcquisition::motorPositions;

DataAcquisition::DataAcquisition(QObject *parent) :
   QThread(parent)
{
   collecting = false;
   biasRefreshing = false;
   monitoring = false;
   biasOn = false;
   biasRefreshRequired = false;
   setAbort(false);
   gigEDetector = DetectorFactory::instance()->getGigEDetector();
   detectorState = GigEDetector::IDLE;
   hv = VoltageSourceFactory::instance()->getHV();
   tdp = 0.0;
   currentImageNumber = 0;
   daqStatus = DataAcquisitionStatus();
   qRegisterMetaType<DataAcquisitionStatus>("DataAcquisitionStatus");
   qRegisterMetaType<DataAcquisitionDefinition>("DataAcquisitionDefinition");
   qRegisterMetaType<GigEDetector::DetectorCommand>("GigEDetector::DetectorCommand");
   qRegisterMetaType<GigEDetector::DetectorState>("GigEDetector::DetectorState");
   busy = false;
   waitingForTrigger = false;
   collectingTriggered = false;
   rdaql.append(this);
}

void DataAcquisition::positionChanged(Motor *motor, const QVariant & value)
{
   int position = value.toInt();
   QString name = motor->property("objectName").toString();
   motorPositions[name] = position;
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

   if (biasOn)
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

void DataAcquisition::initHexitechProcessor()
{
   unsigned int iDebugLevel = 0;
   unsigned int iHistoStartVal = 0;
   unsigned int iHistoEndVal = 10000;
   unsigned int iHistoBins = 1000;
   unsigned int iInterpolationThreshold = 0;
   double gGlobalThreshold = -1.0;
   string sGradientsFile = "";   //"C:\\Temp\\CA_develop\\Gradients.txt";
   string sInterceptsFile = "";   //C:\\Temp\\CA_develop\\Intercepts.txt";
   bool bEnableInCorrector = false;
   bool bEnableCabCorrector = false;
   bool bEnableCsaspCorrector = false;
   bool bEnableCsdCorrector = true;
   bool bEnableIdCorrector = true;
   bool bEnableIpCorrector = false;
   bool bEnableDbPxlsCorrector = false;

   hxtProcessor->setDebugLevel(iDebugLevel);
   hxtProcessor->setHistoStartVal(iHistoStartVal);
   hxtProcessor->setHistoEndVal(iHistoEndVal);
   hxtProcessor->setHistoBins(iHistoBins);
   hxtProcessor->setInterpolationThreshold(iInterpolationThreshold);
   hxtProcessor->setGradientsFile(sGradientsFile);
   hxtProcessor->setInterceptsFile(sInterceptsFile);
   hxtProcessor->setGlobalThreshold(gGlobalThreshold);
   hxtProcessor->setEnableInCorrector(bEnableInCorrector);
   hxtProcessor->setEnableCabCorrector(bEnableCabCorrector);
   hxtProcessor->setEnableCsaspCorrector(bEnableCsaspCorrector);
   hxtProcessor->setEnableCsdCorrector(bEnableCsdCorrector);
   hxtProcessor->setEnableIdCorrector(bEnableIdCorrector);
   hxtProcessor->setEnableIpCorrector(bEnableIpCorrector);
   hxtProcessor->setEnableDbPxlsCorrector(bEnableDbPxlsCorrector);
}

DataAcquisition::~DataAcquisition()
{
   hv->off();
   if (mode == GigEDetector::SOFT_TRIGGER || mode == GigEDetector::EXTERNAL_TRIGGER)
   {
      emit executeCommand(GigEDetector::STOP_TRIGGER, 0, 0);
      Sleep(1000);
   }
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

bool DataAcquisition::isWaitingForTrigger()
{
   return waitingForTrigger;
}

bool DataAcquisition::isCollectingTriggered()
{
   return collectingTriggered;
}

void DataAcquisition::run()
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
      performContinuousDataCollection();
      // TODO : would emiting this to DataAcquisition be better for thread safety
      changeDAQStatus(daqStatus.getMajorStatus(),
                      DataAcquisitionStatus::DONE);
      changeDAQStatus(DataAcquisitionStatus::IDLE,
                      DataAcquisitionStatus::READY);
   }
   else if (mode == GigEDetector::SOFT_TRIGGER ||
            mode == GigEDetector::EXTERNAL_TRIGGER)
   {
      performTriggeredDataCollection();
      // TODO : would emiting this to DataAcquisition be better for thread safety
      changeDAQStatus(daqStatus.getMajorStatus(),
                      DataAcquisitionStatus::WAITING_TRIGGER);
   }
   else if (mode == GigEDetector::FIXED)
   {
      performFixedDataCollection();
      // TODO : would emiting this to DataAcquisition be better for thread safety
      changeDAQStatus(DataAcquisitionStatus::IDLE,
                      daqStatus.getMinorStatus());
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

void DataAcquisition::performContinuousDataCollection()
{
   int nDaq;
   int repeatCount;
   int nDaqOverall = 0;

   emit storeBiasSettings();
   emit disableBiasRefresh();
   emit disableMonitoring();
//   waitForMonitoringDone();

   for (repeatCount = 0; repeatCount < nRepeat; repeatCount++)
   {
      totalFramesAcquired = 0;
      emit appendTimestamp(true);
      setDirectory(repeatCount);
      performMonitorEnvironmentalValues();

      for (nDaq = 0; nDaq < splitDataCollections ; nDaq++)
      {
         if (nDaq > 0)
         {
            emit appendTimestamp(false);
         }
         setDataAcquisitionTime(nDaq);
         collecting = true;

         emit executeCommand(GigEDetector::COLLECT, dataAcquisitionDefinition->getRepeatCount(), nDaqOverall);
         nDaqOverall++;

         waitForCollectingDone();
         daqStatus.setCurrentImage(nDaqOverall);
         if (abortRequired())
            break;

         if (nDaq < (splitDataCollections - 1) ||
             !repeatPauseRequired(repeatCount))
         {
            performMonitorEnvironmentalValues();
            performSingleBiasRefresh();
            if (abortRequired())
               break;
         }
      }

      emit imageComplete(totalFramesAcquired);

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

void DataAcquisition::initialiseTriggeredDataCollection()
{
   int ndaq = 0;

   emit storeBiasSettings();
   emit disableBiasRefresh();
   setDataAcquisitionTime(0);
   biasRefreshRequired = true;
   setDirectory(0);
   emit executeCommand(GigEDetector::COLLECT, 1, ndaq);
}

void DataAcquisition::performTriggeredDataCollection()
{
   int nDaq;

   emit storeBiasSettings();
   emit disableBiasRefresh();
   collectingTriggered = true;

   for (nDaq = 0; nDaq < splitDataCollections; nDaq++)
   {
      setDataAcquisitionTime(nDaq);

      collecting = true;
      if (mode == GigEDetector::SOFT_TRIGGER)
      {
         emit executeCommand(GigEDetector::TRIGGER, 1, 1); // Args 2 & 3 not used by GigEDetector
      }

      waitForCollectingDone();
      daqStatus.setCurrentImage(nDaq + 1);
      if (abortRequired())
         break;

      performSingleBiasRefresh();
      if (abortRequired())
         break;
   }

   gigEDetector->setDataAcquisitionDuration(dataAcquisitionDefinition->getDuration());
   emit restoreBiasSettings();
   collectingTriggered = false;
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

   dataCollectionTime = biasRefreshDataCollectionTime;

   if (nDaq == (splitDataCollections - 1))
   {
      dataCollectionTime = finalDataCollectionTime;
   }

   if (nDaq != 0)
   {
//      dataCollectionTime -= gigEDetector->getTimeError();
   }

   gigEDetector->setDataAcquisitionDuration(dataCollectionTime);
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
      for (int i = 0; i < pauseDuration; i+=1000)
      {
         sleep(1);
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
      // Create hxtProcessing object
      hxtProcessor = ProcessingWindow::getHxtProcessor();
      //hxtProcessor = new hexitech::HxtProcessing("Test", 0);
      // Initialise hxtProcessing object
      //initHexitechProcessor();
   }
   return daqInstance;
}

int DataAcquisition::waitForBiasRefreshDone()
{
   int status = 0;

   while (biasRefreshing)
      sleep(1);

   return status;
}

int DataAcquisition::waitForCollectingDone()
{
   int status = 0;
   int elapsed = 0;
   int percentage = 0;

   while (collecting)
   {
      sleep(1);
      if (daqStatus.getMinorStatus() == DataAcquisitionStatus::COLLECTING &&
          mode == GigEDetector::CONTINUOUS)
      {
         elapsed++;
         percentage = (100000.0 * (double) elapsed / dataCollectionTime) + 0.5;
         if (percentage > 100)
         {
            percentage = 100;
         }
         daqStatus.setPercentage(percentage);
         emit dataAcquisitionStatusChanged(daqStatus);
      }
   }
   qDebug() << "DAQ finished";

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
      //qDebug() << "RELEASING";
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
      waitingForTrigger = false;
      changeDAQStatus(DataAcquisitionStatus::ACQUIRING_DATA,
                      DataAcquisitionStatus::COLLECTING);
      break;
   case GigEDetector::WAITING_TRIGGER:
      waitingForTrigger = true;
      changeDAQStatus(daqStatus.getMajorStatus(),
                      DataAcquisitionStatus::WAITING_TRIGGER);
      // restore bias refreshing after soft trigger initialisation.
      if (biasRefreshRequired)
      {
         emit restoreBiasSettings();
         biasRefreshRequired = false;
      }
      break;
   case GigEDetector::TRIGGERING_STOPPED:
      changeDAQStatus(daqStatus.getMajorStatus(),
                      DataAcquisitionStatus::TRIGGERING_STOPPED);
      changeDAQStatus(DataAcquisitionStatus::IDLE,
                      DataAcquisitionStatus::READY);
      waitingForTrigger = false;
      break;

   }
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
      configureDataCollection();
      start();
   }
}

void DataAcquisition::handleCollectFixedImages()
{
   configureBasicCollection();
   start();
}

void DataAcquisition::initTrigger()
{
   handleInitTrigger();
}

void DataAcquisition::handleInitTrigger()
{
   reservation = ObjectReserver::instance()->reserveForGUI(rdaql);
   if (reservation.getReserved().isEmpty())
   {
      qDebug() << "handleInitTrigger Could not reserve all objects, message = " << reservation.getMessage();
   }
   else
   {
      //qDebug() << "handleInitTrigger called.";
      configureDataCollection();
      initialiseTriggeredDataCollection();
   }
}

void DataAcquisition::trigger()
{
   handleTrigger();
}

void DataAcquisition::handleExternalTriggerReceived()
{
   reservation = ObjectReserver::instance()->reserveForGUI(rdaql);
   if (reservation.getReserved().isEmpty())
   {
      qDebug() << "handleCollectReducedImages Could not reserve all objects, message = " << reservation.getMessage();
   }
   else
   {
      start();
   }
}

void DataAcquisition::handleBufferReady(unsigned char *transferBuffer, unsigned long validFrames)
{
   if (mode != GigEDetector::FIXED && mode != GigEDetector::GIGE_DEFAULT)
   {
      hxtProcessor->pushTransferBuffer(transferBuffer, validFrames);
   }
}

void DataAcquisition::handleImageStarted(char *path, int frameSize)
{
   hxtProcessor->pushRawFileName(path, frameSize);
   hxtProcessor->pushMotorPositions(&motorPositions);
}

void DataAcquisition::handleImageComplete(unsigned long long framesAcquired)
{
   totalFramesAcquired += framesAcquired;
}

void DataAcquisition::handleInitialiseDetector()
{
   gigEDetector->initialiseConnection();
   emit enableMonitoring();
}

void DataAcquisition::handleTrigger()
{
   reservation = ObjectReserver::instance()->reserveForGUI(rdaql);
   if (reservation.getReserved().isEmpty())
   {
      qDebug() << "handleCollectReducedImages Could not reserve all objects, message = " << reservation.getMessage();
   }
   else
   {
      start();
   }
}

void DataAcquisition::stopTrigger()
{
   handleStopTrigger();
}

void DataAcquisition::handleStopTrigger()
{
   reservation = ObjectReserver::instance()->reserveForGUI(rdaql);
   if (reservation.getReserved().isEmpty())
   {
      qDebug() << "handleCollectReducedImages Could not reserve all objects, message = " << reservation.getMessage();
   }
   else
   {
      emit executeCommand(GigEDetector::STOP_TRIGGER, 0, 0);
   }
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

void DataAcquisition::handleBiasRefreshed(QString time)
{
   changeDAQStatus(daqStatus.getMajorStatus(),
                   storedMinorStatus);
   biasRefreshing = false;
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
   setAbort(true);
   gigEDetector->abort(true);
}

void DataAcquisition::setAbort(bool abort)
{
   this->abort = abort;
}

bool DataAcquisition::abortRequired()
{
   return abort;
}
