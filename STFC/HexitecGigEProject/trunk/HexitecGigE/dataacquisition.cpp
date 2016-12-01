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
#include <QDateTime>

DataAcquisition *DataAcquisition::daqInstance = 0;
hexitech::HxtProcessing *DataAcquisition::hxtProcessor = 0;
double DataAcquisition::motorPosition = 0;
QHash<QString, int> DataAcquisition::motorPositions;

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

void DataAcquisition::positionChanged(Motor *motor, const QVariant & value)
{
   int position = value.toInt();
   QString name = motor->property("objectName").toString();
   motorPositions[name] = position;
}

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
         performTriggeredDataCollection();
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

void DataAcquisition::performContinuousDataCollection()
{
   int nDaq;
   int repeatCount;
   int nDaqOverall = 0;

   emit storeBiasSettings();
   emit disableBiasRefresh();
   emit disableMonitoring();
//   waitForMonitoringDone();
   dataAcquisitionModel = DataAcquisitionModel::getInstance();
//   qDebug() << "performContinuousDataCollection() ";

   for (repeatCount = 0; repeatCount < nRepeat; repeatCount++)
   {
      totalFramesAcquired = 0;
      emit appendTimestamp(true);
      setDirectory(repeatCount);
      emit imageStarting(dataAcquisitionModel->getDaqCollectionDuration()/1000, repeatCount, nRepeat);
      performMonitorEnvironmentalValues();

      if (biasPriority)
      {
         qDebug() << "Standard DAQ";
         nDaqOverall = doSplitDataCollections(nDaqOverall, repeatCount);
      }
      else
      {
         qDebug() << "LowPriorityBias DAQ";
         nDaqOverall = doLowPriorityBiasDataCollections(nDaqOverall);
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

int DataAcquisition::doSplitDataCollections(int nDaqOverall, int repeatCount, bool triggering, int ttlInput)
{
   bool suspendTriggering = false;

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
         if (triggering)
         {
            triggered = false;
            changeDAQStatus(daqStatus.getMajorStatus(),
                            DataAcquisitionStatus::WAITING_TRIGGER);
            emit executeCommand(GigEDetector::COLLECT, dataAcquisitionDefinition->getRepeatCount(), nDaqOverall);
            waitForTrigger();
            collecting = true;
            changeDAQStatus(daqStatus.getMajorStatus(),
                            DataAcquisitionStatus::COLLECTING);
         }
         else
         {
            collecting = true;
            emit executeCommand(GigEDetector::COLLECT,
                                dataAcquisitionDefinition->getRepeatCount(), nDaqOverall);
         }
      }

      nDaqOverall++;
      waitForCollectingDone();

      daqStatus.setCurrentImage(nDaqOverall);
      if (abortRequired())
      {
         break;
      }

      if ((nDaq == 0) && triggering && (ttlInput == GigEDetector::INPUT2))
      {
         suspendTriggering = true;
         qDebug() <<"Triggering suspended!";
         emit executeCommand(GigEDetector::CONFIGURE, suspendTriggering, 0);
         waitForConfiguringDone();
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
      else if (triggering && (ttlInput == GigEDetector::INPUT2))
      {
         suspendTriggering = false;
         qDebug() <<"Triggering reconfigured!";
         emit executeCommand(GigEDetector::CONFIGURE, suspendTriggering, 0);
         waitForConfiguringDone();
      }
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
 //        emit executeSingleBiasRefresh();
 //        qDebug() << QTime::currentTime().toString() << "emitted the refresh signal";
         waitForBiasRefreshDone();
      }
      startOfImage = false;
      sleep(0.1);
   }

   daqStatus.setCurrentImage(nDaqOverall);

   return nDaqOverall;
}
/*
void DataAcquisition::performTriggeredDataCollection()
{
   int nDaq;
   int repeatCount;
   int nDaqOverall = 0;

   emit storeBiasSettings();
   emit disableBiasRefresh();
   emit disableMonitoring();
//   waitForMonitoringDone();
   dataAcquisitionModel = DataAcquisitionModel::getInstance();
   qDebug() << "performTriggeredDataCollection()!!! ";

   for (repeatCount = 0; repeatCount < nRepeat; repeatCount++)
   {
      totalFramesAcquired = 0;
      emit appendTimestamp(true);
      setDirectory(repeatCount);
      emit imageStarting(dataAcquisitionModel->getDaqCollectionDuration()/1000, repeatCount, nRepeat);
      performMonitorEnvironmentalValues();

      for (nDaq = 0; nDaq < splitDataCollections ; nDaq++)
      {
         if (nDaq > 0)
         {
            emit appendTimestamp(false);
         }
         setDataAcquisitionTime(nDaq);
         collecting = true;
         triggered = false;

         qDebug() << "Changing DAQStatus to DataAcquisitionStatus::WAITING_TRIGGER";
         changeDAQStatus(daqStatus.getMajorStatus(),
                         DataAcquisitionStatus::WAITING_TRIGGER);

         emit executeCommand(GigEDetector::COLLECT, dataAcquisitionDefinition->getRepeatCount(), nDaqOverall);
         qDebug() <<"GigEDetector::COLLECT command emitted, waiting for a trigger event";
         nDaqOverall++;
         waitForTrigger();

         qDebug() << "Changing DAQStatus to DataAcquisitionStatus::COLLECTING";
         changeDAQStatus(daqStatus.getMajorStatus(),
                         DataAcquisitionStatus::COLLECTING);

         waitForCollectingDone();
         daqStatus.setCurrentImage(nDaqOverall);
         if (abortRequired())
            break;

         if (nDaq < (splitDataCollections - 1) ||
             !repeatPauseRequired(repeatCount))
         {
            performMonitorEnvironmentalValues();
            performSingleBiasRefresh();
//            if (abortRequired())
//               break;
         }
      }

      emit imageComplete(totalFramesAcquired);

      // Break the outer loop too.
//      if (abortRequired())
//         break;
   }

   daqStatus.setCurrentImage(++nDaqOverall);
   emit dataAcquisitionStatusChanged(daqStatus);

   gigEDetector->setDataAcquisitionDuration(dataAcquisitionDefinition->getDuration());
   emit restoreBiasSettings();
   emit enableMonitoring();
}
*/
void DataAcquisition::performTriggeredDataCollection()
{
   int nDaq;
   int repeatCount;
   int nDaqOverall = 0;

   emit storeBiasSettings();
   emit disableBiasRefresh();
   emit disableMonitoring();
//   waitForMonitoringDone();
   dataAcquisitionModel = DataAcquisitionModel::getInstance();
   qDebug() << "performTriggeredDataCollection()!!! ";

   for (repeatCount = 0; repeatCount < nRepeat; repeatCount++)
   {
      totalFramesAcquired = 0;
      emit appendTimestamp(true);
      setDirectory(repeatCount);
      emit imageStarting(dataAcquisitionModel->getDaqCollectionDuration()/1000, repeatCount, nRepeat);
      performMonitorEnvironmentalValues();

      nDaqOverall = doSplitDataCollections(nDaqOverall, repeatCount, true);
/*      for (nDaq = 0; nDaq < splitDataCollections ; nDaq++)
      {
         if (nDaq > 0)
         {
            emit appendTimestamp(false);
         }
         setDataAcquisitionTime(nDaq);
         collecting = true;
         triggered = false;

         qDebug() << "Changing DAQStatus to DataAcquisitionStatus::WAITING_TRIGGER";
         changeDAQStatus(daqStatus.getMajorStatus(),
                         DataAcquisitionStatus::WAITING_TRIGGER);

         emit executeCommand(GigEDetector::COLLECT, dataAcquisitionDefinition->getRepeatCount(), nDaqOverall);
         qDebug() <<"GigEDetector::COLLECT command emitted, waiting for a trigger event";
         nDaqOverall++;
         waitForTrigger();

         qDebug() << "Changing DAQStatus to DataAcquisitionStatus::COLLECTING";
         changeDAQStatus(daqStatus.getMajorStatus(),
                         DataAcquisitionStatus::COLLECTING);

         waitForCollectingDone();
         daqStatus.setCurrentImage(nDaqOverall);
         if (abortRequired())
            break;

         if (nDaq < (splitDataCollections - 1) ||
             !repeatPauseRequired(repeatCount))
         {
            performMonitorEnvironmentalValues();
            performSingleBiasRefresh();
//            if (abortRequired())
//               break;
         }
      }
*/
      //ToDo Need to handle low priority bias with doLowPriorityBiasDataCollections(nDaqOverall) ???
      qDebug() << "emit imageComplete() called from performTriggeredDataCollection(): totalFramesAcquired = " << totalFramesAcquired;
      emit imageComplete(totalFramesAcquired);

      // Break the outer loop too.
//      if (abortRequired())
//         break;
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
//   qDebug() << "DataAcquisition::performSingleBiasRefresh() called!!!, threadId: " << QThread::currentThreadId();
   if (biasOn)
   {
//      qDebug() << "biasOn!!!";
      changeDAQStatus(daqStatus.getMajorStatus(), DataAcquisitionStatus::BIAS_REFRESHING);
      biasRefreshing = true;
      emit executeSingleBiasRefresh();
//      qDebug() << QTime::currentTime().toString() << "emitted the refresh signal";
      waitForBiasRefreshDone();
//      qDebug() << QTime::currentTime().toString() << "bias refresh done.";
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

   changeDAQStatus(daqStatus.getMajorStatus(), DataAcquisitionStatus::BIAS_REFRESHING);
//   qDebug() << QTime::currentTime().toString() << "waitForBiasRefreshDone(), biasRefreshing: " << biasRefreshing;
   while (biasRefreshing)
   {
      sleep(0.1);
   }
//   qDebug() << QTime::currentTime().toString() << "waiting complete!!!, biasRefreshing: " << biasRefreshing;

   return status;
}

int DataAcquisition::waitForTrigger()
{
   int status = 0;

   changeDAQStatus(daqStatus.getMajorStatus(),
                   DataAcquisitionStatus::WAITING_TRIGGER);

   while (!triggered)
      sleep(0.1);

   return status;

}

int DataAcquisition::waitForConfiguringDone()
{
   int status = 0;

   qDebug() << "DataAcquisition::waitForConfiguringDone()";
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

//   qDebug() << QTime::currentTime().toString() <<" collecting: " << collecting << "daqStatus.getMinorStatus(): " << daqStatus.getMinorStatus() << " mode: " << mode;
   while (collecting)
   {
      sleep(1);
      if (daqStatus.getMinorStatus() == DataAcquisitionStatus::COLLECTING &&
          mode == GigEDetector::CONTINUOUS)
      {
         elapsed++;
         if (biasPriority)
         {
            percentage = (100000.0 * (double) elapsed / dataCollectionTime) + 0.5;
         }
         else
         {
            remainingFrames = gigEDetector->getRemainingFrames();
            percentage = ((double)totalImageFrames - (double)remainingFrames) / (double)totalImageFrames * 100.0;

            qDebug() << " totalImageFrames: " << totalImageFrames << " remainingFrames: " << remainingFrames;
         }
         if (percentage > 100)
         {
            percentage = 100;
         }
         daqStatus.setPercentage(percentage);
         emit dataAcquisitionStatusChanged(daqStatus);
      }
   }
   if ((remainingFrames = gigEDetector->getRemainingFrames()) != 0)
   {
      percentage = ((double)totalImageFrames - (double)remainingFrames) / (double)totalImageFrames * 100.0;
   }
   else
   {
      percentage = 100;
      qDebug() << QTime::currentTime().toString() << "DAQ finished";
   }
   daqStatus.setPercentage(percentage);
   emit dataAcquisitionStatusChanged(daqStatus);

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
      qDebug() <<"handleTriggeringSelectionChanged(int triggering):" << triggeringMode;
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
      triggered = true;
      hxtProcessor->pushTransferBuffer(transferBuffer, validFrames);
   }
   ///  HexitecGigE Addition:
   /// Must provide a set of motorPositions also,
   /// or bufferQueue fills up while motorQueue remains empty..
   hxtProcessor->pushMotorPositions(&motorPositions);
}

void DataAcquisition::handleImageStarted(char *path, int frameSize)
{
   qDebug() << "DataAcquisition::handleImageStarted path: " << path;
   hxtProcessor->pushRawFileName(path, frameSize);
//   hxtProcessor->pushMotorPositions(&motorPositions); /// Provide motorPositions together with buffer instead
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
   qDebug() << "DataAcquisition::handleCancelReducedDataCollection()";
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
