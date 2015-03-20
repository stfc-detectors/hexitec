#include "dataacquisitionmodel.h"
#include "voltageSourceFactory.h"
#include "detectorfactory.h"
#include "dataacquisition.h"
#include "processingwindow.h"

DataAcquisitionModel *DataAcquisitionModel::damInstance = 0;

DataAcquisitionModel::DataAcquisitionModel(DataAcquisitionForm *dataAcquisitionForm,
                                           DetectorControlForm *detectorControlForm,
                                           QObject *parent) :
   QObject(parent)
{
   DetectorFilename *dataFilename = dataAcquisitionDefinition.getDataFilename();
   DetectorFilename *logFilename = dataAcquisitionDefinition.getLogFilename();

   this->dataAcquisitionForm = dataAcquisitionForm;
   this->detectorControlForm = detectorControlForm;

   keithley = VoltageSourceFactory::instance()->getKeithley();
   aspectDetector = DetectorFactory::instance()->getAspectDetector();
   detectorMonitor = DetectorFactory::instance()->getDetectorMonitor();
   dataAcquisition = DataAcquisition::instance();
   objectReserver = ObjectReserver::instance();

   dataAcquisitionForm->setModes(aspectDetector->getReducedDataModes());

   connectDataAcquisitionForm();
   connectDetectorControlForm();
   connectDetectorMonitor();
   connectDataAcquisition();
   connectAspectDetector();
   connectDataAcquisitionModel();
   connectKeithley();
   connectObjectReserver();

   initialiseDetectorFilename(dataFilename);
   initialiseDetectorFilename(logFilename);
   rdaqml.append((QObject *)this);
}

DataAcquisitionModel::~DataAcquisitionModel()
{
}

DataAcquisitionModel *DataAcquisitionModel::instance(DataAcquisitionForm *dataAcquisitionForm, DetectorControlForm *detectorControlForm, QObject *parent)
{
   if (damInstance == 0)
   {
      damInstance = new DataAcquisitionModel(dataAcquisitionForm, detectorControlForm, parent);
   }

   return damInstance;
}

DataAcquisitionModel *DataAcquisitionModel::getInstance()
{
   return damInstance;
}

QList<QObject *> DataAcquisitionModel::getReserveList()
{
   return rdaqml;
}

void DataAcquisitionModel::connectDetectorMonitor()
{
   connect(detectorMonitor, SIGNAL(updateMonitorData(MonitorData *)), dataAcquisitionForm, SLOT(handleMonitorData(MonitorData *)));
   connect(detectorMonitor, SIGNAL(updateMonitorData(MonitorData *)), detectorControlForm, SLOT(handleMonitorData(MonitorData *)));
   connect(detectorMonitor, SIGNAL(updateMonitorData(MonitorData *)), dataAcquisition, SLOT(handleMonitorData(MonitorData *)));
   connect(detectorMonitor, SIGNAL(writeError(QString)), ApplicationOutput::instance(), SLOT(writeError(QString)));
   connect(detectorMonitor, SIGNAL(writeMessage(QString)), ApplicationOutput::instance(), SLOT(writeMessage(QString)));
   connect(detectorMonitor, SIGNAL(temperatureBelowDP()), keithley, SLOT(handleTemperatureBelowDP()));
   connect(detectorMonitor, SIGNAL(temperatureBelowDP()), detectorControlForm, SLOT(handleTemperatureBelowDP()));
   connect(detectorMonitor, SIGNAL(temperatureBelowDP()), dataAcquisition, SLOT(handleAbortDAQ()));
   connect(detectorMonitor, SIGNAL(temperatureAboveDP()), detectorControlForm, SLOT(handleTemperatureAboveDP()));
   detectorMonitor->start();
}

void DataAcquisitionModel::connectDataAcquisition()
{
   connect(dataAcquisition, SIGNAL(executeCommand(AspectDetector::DetectorCommand, int, int)),
           aspectDetector, SLOT(handleExecuteCommand(AspectDetector::DetectorCommand, int, int)));
   connect(dataAcquisition, SIGNAL(executeOffsets()),
           aspectDetector, SLOT(handleExecuteOffsets()));
   connect(dataAcquisition, SIGNAL(executeReducedDataCollection()),
           aspectDetector, SLOT(handleReducedDataCollection()));
   connect(dataAcquisition, SIGNAL(executeSingleBiasRefresh()),
           keithley, SLOT(executeSingleBiasRefresh()));
   connect(dataAcquisition, SIGNAL(storeBiasSettings()),
           keithley, SLOT(storeBiasSettings()));
   connect(dataAcquisition, SIGNAL(restoreBiasSettings()),
           keithley, SLOT(restoreBiasSettings()));
   connect(dataAcquisition, SIGNAL(disableBiasRefresh()),
           keithley, SLOT(handleDisableBiasRefresh()));
   connect(dataAcquisition, SIGNAL(collectingChanged(bool)),
           dataAcquisitionForm, SLOT(handleCollectingChanged(bool)));
   connect(dataAcquisition, SIGNAL(collectingChanged(bool)),
           detectorControlForm, SLOT(handleCollectingChanged(bool)));
   connect(dataAcquisition, SIGNAL(dataAcquisitionStatusChanged(DataAcquisitionStatus)),
           dataAcquisitionForm, SLOT(handleDataAcquisitionStatusChanged(DataAcquisitionStatus)));
   connect(dataAcquisition, SIGNAL(dataAcquisitionStatusChanged(DataAcquisitionStatus)),
           detectorControlForm, SLOT(handleDataAcquisitionStatusChanged(DataAcquisitionStatus)));
   connect(dataAcquisition, SIGNAL(dataAcquisitionStatusChanged(DataAcquisitionStatus)),
           ProcessingWindow::getHxtProcessor(), SLOT(handleDataAcquisitionStatusChanged(DataAcquisitionStatus)));
}

void DataAcquisitionModel::connectAspectDetector()
{
   connect(aspectDetector, SIGNAL(writeMessage(QString)), ApplicationOutput::instance(), SLOT(writeMessage(QString)));
   connect(aspectDetector, SIGNAL(writeError(QString)), ApplicationOutput::instance(), SLOT(writeError(QString)));
   connect(aspectDetector, SIGNAL(notifyMode(AspectDetector::Mode)), dataAcquisitionForm, SLOT(handleModeChanged(AspectDetector::Mode)));
   connect(aspectDetector, SIGNAL(notifyMode(AspectDetector::Mode)), detectorControlForm, SLOT(handleModeChanged(AspectDetector::Mode)));
   connect(aspectDetector, SIGNAL(notifyMode(AspectDetector::Mode)), dataAcquisition, SLOT(handleModeChanged(AspectDetector::Mode)));
   connect(aspectDetector, SIGNAL(notifyState(AspectDetector::DetectorState)), dataAcquisition, SLOT(receiveState(AspectDetector::DetectorState)));
   connect(aspectDetector, SIGNAL(image1Acquired(QPixmap)), detectorControlForm, SLOT(setPixmap1(QPixmap)));
   connect(aspectDetector, SIGNAL(image2Acquired(QPixmap)), detectorControlForm, SLOT(setPixmap2(QPixmap)));
   connect(aspectDetector, SIGNAL(image3Acquired(QPixmap)), detectorControlForm, SLOT(setPixmap3(QPixmap)));
   connect(aspectDetector, SIGNAL(imageAcquired(QPixmap)), detectorControlForm, SLOT(setPixmap(QPixmap)));
   connect(aspectDetector, SIGNAL(prepareForOffsets()), dataAcquisitionForm, SLOT(prepareForOffsets()));
   connect(aspectDetector, SIGNAL(prepareForDataCollection()), dataAcquisitionForm, SLOT(prepareForDataCollection()));
   connect(aspectDetector, SIGNAL(externalTriggerReceived()), dataAcquisition, SLOT(handleExternalTriggerReceived()));
}

void DataAcquisitionModel::connectDetectorControlForm()
{
   connect(detectorControlForm, SIGNAL(executeCommand(Keithley::VoltageSourceCommand)),
           keithley, SLOT(handleExecuteCommand(Keithley::VoltageSourceCommand)));
   connect(detectorControlForm, SIGNAL(executeCommand(AspectDetector::DetectorCommand, int, int)),
           aspectDetector, SLOT(handleExecuteCommand(AspectDetector::DetectorCommand, int, int)));
   connect(detectorControlForm, SIGNAL(collectImagesPressed()),
           dataAcquisition, SLOT(handleCollectFixedImages()));
   connect(detectorControlForm, SIGNAL(abortDAQPressed()),
           dataAcquisition, SLOT(handleAbortDAQ()));
   connect(detectorControlForm, SIGNAL(fixedImageCountChanged(int)),
           this, SLOT(handleFixedImageCountChanged(int)));
   connect(detectorControlForm, SIGNAL(setFingerTemperature(double)),
           dataAcquisition, SLOT(handleSetFingerTemperature(double)));
   connect(detectorControlForm, SIGNAL(biasVoltageChanged(bool)),
           this, SLOT(handleBiasVoltageChanged(bool)));
}

void DataAcquisitionModel::connectDataAcquisitionModel()
{
   connect(this, SIGNAL(dataChanged(DataAcquisitionDefinition)),
           dataAcquisitionForm, SLOT(handleDataChanged(DataAcquisitionDefinition)));
   connect(this, SIGNAL(dataChanged(QString)),
           dataAcquisitionForm, SLOT(handleDataChanged(QString)));
   connect(this, SIGNAL(daqDurationChanged(double)),
           dataAcquisitionForm, SLOT(handleDaqDurationChanged(double)));
   connect(this, SIGNAL(biasVoltageChanged(bool)),
           detectorControlForm, SLOT(handleBiasVoltageChanged(bool)));
}

void DataAcquisitionModel::connectDataAcquisitionForm()
{
   connect(dataAcquisitionForm, SIGNAL(executeCommand(AspectDetector::DetectorCommand, int)),
           aspectDetector, SLOT(handleExecuteCommand(AspectDetector::DetectorCommand, int)));
   connect(dataAcquisitionForm, SIGNAL(dataFilenameChanged(DetectorFilename)),
           this, SLOT(handleDataFilenameChanged(DetectorFilename)));
   connect(dataAcquisitionForm, SIGNAL(logFilenameChanged(DetectorFilename)),
           this, SLOT(handleLogFilenameChanged(DetectorFilename)));
   connect(dataAcquisitionForm, SIGNAL(dataAcquisitionDefinitionChanged(DataAcquisitionDefinition)),
           this, SLOT(handleDataAcquisitionDefinitionChanged(DataAcquisitionDefinition)));
   connect(dataAcquisitionForm, SIGNAL(collectImagesPressed()),
           dataAcquisition, SLOT(handleCollectReducedImages()));
   connect(dataAcquisitionForm, SIGNAL(initTriggerPressed()),
           dataAcquisition, SLOT(handleInitTrigger()));
   connect(dataAcquisitionForm, SIGNAL(triggerPressed()),
           dataAcquisition, SLOT(handleTrigger()));
   connect(dataAcquisitionForm, SIGNAL(stopTriggerPressed()),
           dataAcquisition, SLOT(handleStopTrigger()));
   connect(dataAcquisitionForm, SIGNAL(abortDAQPressed()),
           dataAcquisition, SLOT(handleAbortDAQ()));
   connect(dataAcquisitionForm, SIGNAL(executeOffsets()),
           dataAcquisition, SLOT(handleExecuteOffsets()));
   connect(dataAcquisitionForm, SIGNAL(cancelOffsets()),
           dataAcquisition, SLOT(handleCancelOffsets()));
   connect(dataAcquisitionForm, SIGNAL(executeReducedDataCollection()),
           dataAcquisition, SLOT(handleExecuteReducedDataCollection()));
   connect(dataAcquisitionForm, SIGNAL(cancelReducedDataCollection()),
           dataAcquisition, SLOT(handleCancelReducedDataCollection()));
   connect(dataAcquisitionForm, SIGNAL(createLogFile(DetectorFilename *)),
           detectorMonitor, SLOT(createLogFile(DetectorFilename *)));
}

void DataAcquisitionModel::connectKeithley()
{
   connect(keithley, SIGNAL(biasRefreshing()), detectorMonitor, SLOT(handleBiasRefreshing()));
   connect(keithley, SIGNAL(biasRefreshing()), dataAcquisition, SLOT(handleBiasRefreshing()));

   connect(keithley, SIGNAL(biasRefreshed(QString)), dataAcquisitionForm, SLOT(handleBiasRefreshed(QString)));
   connect(keithley, SIGNAL(biasRefreshed(QString)), detectorControlForm, SLOT(handleBiasRefreshed(QString)));
   connect(keithley, SIGNAL(biasRefreshed(QString)), dataAcquisition, SLOT(handleBiasRefreshed(QString)));

   connect(keithley, SIGNAL(biasState(bool)), dataAcquisition, SLOT(handleBiasState(bool)));
   connect(keithley, SIGNAL(biasVoltageChanged(bool)), this, SLOT(handleBiasVoltageChanged(bool)));
}

void DataAcquisitionModel::connectObjectReserver()
{
   connect(objectReserver, SIGNAL(scriptReserve(QString)), detectorControlForm, SLOT(handleScriptReserve(QString)));
   connect(objectReserver, SIGNAL(scriptRelease(QString)), detectorControlForm, SLOT(handleScriptRelease(QString)));
   connect(objectReserver, SIGNAL(scriptRelease(QString)), dataAcquisitionForm, SLOT(handleScriptRelease(QString)));
   connect(objectReserver, SIGNAL(controlledByGui()), dataAcquisitionForm, SLOT(handleControlledByGui()));
}

void DataAcquisitionModel::initialiseDetectorFilename(DetectorFilename *detectorFilename)
{
   detectorFilename->setDirectory(aspectDetector->getDirectory());
   detectorFilename->setPrefix(aspectDetector->getPrefix());
   detectorFilename->setTimestampOn(aspectDetector->getTimestampOn());
   emit dataChanged(dataAcquisitionDefinition);
}

void DataAcquisitionModel::setDetectorFilename(DetectorFilename sourceFilename, DetectorFilename *destFilename)
{
   destFilename->setDirectory(sourceFilename.getDirectory());
   destFilename->setPrefix(sourceFilename.getPrefix());
   destFilename->setTimestampOn(sourceFilename.getTimestampOn());
}

DataAcquisitionDefinition *DataAcquisitionModel::getDataAcquisitionDefinition()
{
   return &dataAcquisitionDefinition;
}

void DataAcquisitionModel::configure(QString directory, QString prefix, bool timestampOn, bool offsets, double duration, int repeatCount, int repeatInterval)
{
   DetectorFilename *dataFilename = dataAcquisitionDefinition.getDataFilename();
   dataFilename->setDirectory(directory);
   dataFilename->setPrefix(prefix);
   dataFilename->setTimestampOn(timestampOn);
   dataAcquisitionDefinition.setDuration(duration * 1000);
   dataAcquisitionDefinition.setRepeatCount(repeatCount);
   dataAcquisitionDefinition.setRepeatInterval(repeatInterval * 1000);
   dataAcquisitionDefinition.setOffsets(offsets);

   //changeDaqDuration(); Commented out as the GUI is doing the DAQ work and a result of the GUI fields being updated. i.e. Scripting changes the model which updates the GUI.
   emit dataChanged(dataAcquisitionDefinition);
}

void DataAcquisitionModel::setDirectory(QString directory)
{
   DetectorFilename *dataFilename = dataAcquisitionDefinition.getDataFilename();
   dataFilename->setDirectory(directory);
   emit dataChanged(dataAcquisitionDefinition);
}

void DataAcquisitionModel::setPrefix(QString prefix)
{
   DetectorFilename *dataFilename = dataAcquisitionDefinition.getDataFilename();
   dataFilename->setPrefix(prefix);
   emit dataChanged(dataAcquisitionDefinition);
}

void DataAcquisitionModel::setMode(QString mode)
{
   if (aspectDetector->getModes().contains(mode))
   {
      emit dataChanged(mode);
   }
}

QString DataAcquisitionModel::getDirectory()
{
   return dataAcquisitionDefinition.getDataFilename()->getDirectory();
}

void DataAcquisitionModel::setTimestampOn(bool timestampOn)
{
   DetectorFilename *dataFilename = dataAcquisitionDefinition.getDataFilename();
   dataFilename->setTimestampOn(timestampOn);
   emit dataChanged(dataAcquisitionDefinition);
}

void DataAcquisitionModel::handleDataFilenameChanged(DetectorFilename dataFilename)
{
   setDetectorFilename(dataFilename, dataAcquisitionDefinition.getDataFilename());
}

void DataAcquisitionModel::handleLogFilenameChanged(DetectorFilename logFilename)
{
   setDetectorFilename(logFilename, dataAcquisitionDefinition.getLogFilename());
}

void DataAcquisitionModel::changeDaqDuration()
{
   int splitDataCollections;
   int repeatCount = dataAcquisitionDefinition.getRepeatCount();
   int repeatInterval = dataAcquisitionDefinition.getRepeatInterval();
   int biasRefreshTime = keithley->getBiasRefreshTime();
   bool biasOn = keithley->getBiasOnState();

   daqDuration = dataAcquisitionDefinition.getDuration();
   splitDataCollections = ceil(((double) daqDuration) / ((double ) keithley->getBiasRefreshInterval()));

   // If bias on add number of bias refreshes per collection
   if (biasOn)
   {
      daqDuration += (splitDataCollections * biasRefreshTime);
   }

   // Add pause time
   daqDuration += repeatInterval;

   // Multiple by the number of repeated collections
   daqDuration *= repeatCount;

   // The last image in the last collection doesn't have bias refresh
   if (biasOn && (daqDuration > 0))
   {
      daqDuration -= keithley->getBiasRefreshTime();
   }

   // The last collection doesn't have a pause
   if (repeatCount > 1)
   {
      daqDuration -= repeatInterval;
   }

   // Convert to seconds
   daqDuration /= 1000;

   emit daqDurationChanged(daqDuration);
}

void DataAcquisitionModel::handleDataAcquisitionDefinitionChanged(DataAcquisitionDefinition dataAcquisitionDefinition)
{
   this->dataAcquisitionDefinition.setDuration(dataAcquisitionDefinition.getDuration());
   this->dataAcquisitionDefinition.setOffsets(dataAcquisitionDefinition.getOffsets());
   this->dataAcquisitionDefinition.setRepeatCount(dataAcquisitionDefinition.getRepeatCount());
   this->dataAcquisitionDefinition.setRepeatInterval(dataAcquisitionDefinition.getRepeatInterval());

   changeDaqDuration();
}

void DataAcquisitionModel::handleFixedImageCountChanged(int fixedImageCount)
{
   this->dataAcquisitionDefinition.setFixedImageCount(fixedImageCount);
}

void DataAcquisitionModel::handleBiasVoltageChanged(bool biasOn)
{
   changeDaqDuration();
   emit biasVoltageChanged(biasOn);
}