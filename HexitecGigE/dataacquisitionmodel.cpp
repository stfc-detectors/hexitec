#include "dataacquisitionmodel.h"
#include "voltageSourceFactory.h"
#include "detectorfactory.h"
#include "dataacquisition.h"
//#include "processingwindow.h"
#include "progressform.h"

DataAcquisitionModel *DataAcquisitionModel::damInstance = 0;

DataAcquisitionModel::DataAcquisitionModel(DataAcquisitionForm *dataAcquisitionForm,
                                           DetectorControlForm *detectorControlForm,
                                           ProgressForm *progressForm,
                                           ProcessingBufferGenerator *processingBufferGenerator,
                                           QObject *parent) :
   QObject(parent)
{
   DetectorFilename *dataFilename = dataAcquisitionDefinition.getDataFilename();
   DetectorFilename *logFilename = dataAcquisitionDefinition.getLogFilename();

   this->dataAcquisitionForm = dataAcquisitionForm;
   this->detectorControlForm = detectorControlForm;
   this->progressForm = progressForm;
   this->processingBufferGenerator = processingBufferGenerator;

   hv = VoltageSourceFactory::instance()->getHV();
   gigEDetector = DetectorFactory::instance()->getGigEDetector();
   detectorMonitor = DetectorFactory::instance()->getDetectorMonitor();
   dataAcquisition = DataAcquisition::instance();
   objectReserver = ObjectReserver::instance();

   connectDataAcquisitionForm();
   connectDetectorControlForm();
   connectDetectorMonitor();
   connectDataAcquisition();
   connectGigEDetector();
   connectDataAcquisitionModel();
   connectHV();
   connectObjectReserver();

   initialiseDetectorFilename(dataFilename);
   initialiseDetectorFilename(logFilename);
   rdaqml.append((QObject *)this);
}

DataAcquisitionModel::~DataAcquisitionModel()
{
}

DataAcquisitionModel *DataAcquisitionModel::instance(DataAcquisitionForm *dataAcquisitionForm, DetectorControlForm *detectorControlForm,
                                                     ProgressForm *progressForm, ProcessingBufferGenerator *processingBufferGenerator, QObject *parent)
{
   if (damInstance == 0)
   {
      damInstance = new DataAcquisitionModel(dataAcquisitionForm, detectorControlForm,
                                             progressForm, processingBufferGenerator, parent);
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

double DataAcquisitionModel::getDaqCollectionDuration()
{
   return daqCollectionDuration;
}

double DataAcquisitionModel::getDaqDuration()
{
   return daqDuration;
}

void DataAcquisitionModel::connectDetectorMonitor()
{
   connect(detectorMonitor, SIGNAL(updateMonitorData(MonitorData *)), dataAcquisitionForm, SLOT(handleMonitorData(MonitorData *)));
   connect(detectorMonitor, SIGNAL(updateMonitorData(MonitorData *)), detectorControlForm, SLOT(handleMonitorData(MonitorData *)));
   connect(detectorMonitor, SIGNAL(updateMonitorData(MonitorData *)), dataAcquisition, SLOT(handleMonitorData(MonitorData *)));
   connect(detectorMonitor, SIGNAL(writeError(QString)), ApplicationOutput::instance(), SLOT(writeError(QString)));
   connect(detectorMonitor, SIGNAL(writeMessage(QString)), ApplicationOutput::instance(), SLOT(writeMessage(QString)));
   connect(detectorMonitor, SIGNAL(temperatureBelowDP()), hv, SLOT(handleTemperatureBelowDP()));
   connect(detectorMonitor, SIGNAL(temperatureBelowDP()), detectorControlForm, SLOT(handleTemperatureBelowDP()));
   connect(detectorMonitor, SIGNAL(temperatureBelowDP()), dataAcquisition, SLOT(handleAbortDAQ()));
   connect(detectorMonitor, SIGNAL(temperatureAboveDP()), detectorControlForm, SLOT(handleTemperatureAboveDP()));
   connect(detectorMonitor, SIGNAL(monitoringDone()), dataAcquisition, SLOT(handleMonitored()));
   detectorMonitor->start();
}

void DataAcquisitionModel::connectDataAcquisition()
{
   connect(dataAcquisition, SIGNAL(executeCommand(GigEDetector::DetectorCommand, int, int)),
           gigEDetector, SLOT(handleExecuteCommand(GigEDetector::DetectorCommand, int, int)));
   connect(dataAcquisition, SIGNAL(executeOffsets()),
           gigEDetector, SLOT(handleExecuteOffsets()));
   connect(dataAcquisition, SIGNAL(executeReducedDataCollection()),
           gigEDetector, SLOT(handleReducedDataCollection()));

   connect(dataAcquisition, SIGNAL(executeSingleBiasRefresh()),
           hv, SLOT(executeSingleBiasRefresh()));
   connect(dataAcquisition, SIGNAL(storeBiasSettings()),
           hv, SLOT(storeBiasSettings()));
   connect(dataAcquisition, SIGNAL(restoreBiasSettings()),
           hv, SLOT(restoreBiasSettings()));
   connect(dataAcquisition, SIGNAL(disableBiasRefresh()),
           hv, SLOT(handleDisableBiasRefresh()));
   connect(dataAcquisition, SIGNAL(enableBiasRefresh()),
           hv, SLOT(handleEnableBiasRefresh()));

   connect(dataAcquisition, SIGNAL(collectingChanged(bool)),
           dataAcquisitionForm, SLOT(handleCollectingChanged(bool)));

   connect(dataAcquisition, SIGNAL(collectingChanged(bool)),
           detectorControlForm, SLOT(handleCollectingChanged(bool)));

   connect(dataAcquisition, SIGNAL(dataAcquisitionStatusChanged(DataAcquisitionStatus)),
           dataAcquisitionForm, SLOT(handleDataAcquisitionStatusChanged(DataAcquisitionStatus)));

   connect(dataAcquisition, SIGNAL(dataAcquisitionStatusChanged(DataAcquisitionStatus)),
           detectorControlForm, SLOT(handleDataAcquisitionStatusChanged(DataAcquisitionStatus)));
   connect(dataAcquisition, SIGNAL(setTargetTemperature(double)),
           gigEDetector, SLOT(handleSetTargetTemperature(double)));
   connect(dataAcquisition, SIGNAL(appendTimestamp(bool)),
           gigEDetector, SLOT(handleAppendTimestamp(bool)));

   connect(dataAcquisition, SIGNAL(executeMonitorEnvironmentalValues()),
           detectorMonitor, SLOT(executeMonitorEnvironmentalValues()));
   connect(dataAcquisition, SIGNAL(enableMonitoring()),
           detectorMonitor, SLOT(enableMonitoring()));
   connect(dataAcquisition, SIGNAL(disableMonitoring()),
           detectorMonitor, SLOT(disableMonitoring()));
   connect(dataAcquisition, SIGNAL(imageComplete(long long)),
           processingBufferGenerator, SLOT(handleImageComplete(long long)));
   connect(dataAcquisition, SIGNAL(transferBufferReady(unsigned char*,ulong)),
              processingBufferGenerator, SLOT(handleTransferBufferReady(unsigned char*,ulong)));

   connect(this->dataAcquisitionForm, SIGNAL(newDataAcquisitionState(QString)),
           this->progressForm, SLOT(handleNewDataAcquisitionState(QString)));

   connect(this->dataAcquisitionForm, SIGNAL(newDataAcquisitionProgressBarValue(int)),
           this->progressForm, SLOT(handleNewDataAcquisitionProgressBarValue(int)));

   connect(this->dataAcquisitionForm, SIGNAL(newDataAcquisitionImageProgressValue(int)),
           this->progressForm, SLOT(handleNewDataAcquisitionImageProgressValue(int)));

   connect(dataAcquisition, SIGNAL(imageStarting(char *, int, int)),
           processingBufferGenerator, SLOT(handleImageStarting(char *, int, int)));
}

void DataAcquisitionModel::connectGigEDetector()
{
   connect(gigEDetector, SIGNAL(writeMessage(QString)), ApplicationOutput::instance(), SLOT(writeMessage(QString)));
   connect(gigEDetector, SIGNAL(writeError(QString)), ApplicationOutput::instance(), SLOT(writeError(QString)));
   connect(gigEDetector, SIGNAL(notifyState(GigEDetector::DetectorState)), dataAcquisition, SLOT(receiveState(GigEDetector::DetectorState)));
   connect(gigEDetector, SIGNAL(imageAcquired(QPixmap)), detectorControlForm, SLOT(setPixmap(QPixmap)));
   connect(gigEDetector, SIGNAL(prepareForOffsets()), dataAcquisitionForm, SLOT(prepareForOffsets()));
   connect(gigEDetector, SIGNAL(prepareForDataCollection()), dataAcquisitionForm, SLOT(prepareForDataCollection()));
   connect(gigEDetector, SIGNAL(imageStarted(char *, int)),
           dataAcquisition, SLOT(handleImageStarted(char *, int)));
   connect(gigEDetector, SIGNAL(imageStarted(char *)),
           processingBufferGenerator, SLOT(handleImageStarted(char *)));
   connect(gigEDetector, SIGNAL(imageComplete(unsigned long long)),
           dataAcquisition, SLOT(handleImageComplete(unsigned long long)));
//   connect(gigEDetector, SIGNAL(executeBufferReady(unsigned char*,ulong)),
//           dataAcquisition, SLOT(handleBufferReady(unsigned char*,ulong));
   connect(gigEDetector, SIGNAL(enableMonitoring()),
           detectorMonitor, SLOT(enableMonitoring()));
   connect(gigEDetector, SIGNAL(executeCommand(GigEDetector::DetectorCommand, int, int)),
           gigEDetector, SLOT(handleExecuteCommand(GigEDetector::DetectorCommand, int, int)));
   connect(gigEDetector, SIGNAL(triggeringAvailableChanged(bool)),
           detectorControlForm, SLOT(handleTriggeringAvailable(bool)));
//   connect(gigEDetector, SIGNAL(detectorResolutionSet(int, int)),
//           processingBufferGenerator, SLOT(handleConfigureSensor(int, int)));
   connect(gigEDetector, SIGNAL(cancelDataCollection()),
           dataAcquisition, SLOT(handleCancelReducedDataCollection()));
}

void DataAcquisitionModel::connectDetectorControlForm()
{
   connect(detectorControlForm, SIGNAL(executeCommand(HV::VoltageSourceCommand)),
           hv, SLOT(handleExecuteCommand(HV::VoltageSourceCommand)));
   connect(detectorControlForm, SIGNAL(executeCommand(GigEDetector::DetectorCommand, int, int)),
           gigEDetector, SLOT(handleExecuteCommand(GigEDetector::DetectorCommand, int, int)));
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
   connect(detectorControlForm, SIGNAL(triggeringSelectionChanged(int)),
           dataAcquisition, SLOT(handleTriggeringSelectionChanged(int)));
   connect(detectorControlForm, SIGNAL(disableMonitoring()),
           detectorMonitor, SLOT(disableMonitoring()));
   connect(detectorControlForm, SIGNAL(disableBiasRefresh()),
           hv, SLOT(handleDisableBiasRefresh()));
   connect(detectorControlForm, SIGNAL(writeMessage(QString)), ApplicationOutput::instance(), SLOT(writeMessage(QString)));
   connect(detectorControlForm, SIGNAL(writeError(QString)), ApplicationOutput::instance(), SLOT(writeError(QString)));
   connect(detectorControlForm, SIGNAL(initialiseDetector()),
           dataAcquisition, SLOT(handleInitialiseDetector()));
   connect(detectorControlForm, SIGNAL(triggeringSelectionChanged(int)),
           dataAcquisitionForm, SLOT(handleTriggeringSelectionChanged(int)));
   connect(detectorControlForm, SIGNAL(ttlInputSelectionChanged(int)),
           dataAcquisitionForm, SLOT(handleTtlInputSelectionChanged(int)));
   connect(detectorControlForm, SIGNAL(setTriggerTimeout(double)),
           gigEDetector, SLOT(handleSetTriggerTimeout(double)));
}

void DataAcquisitionModel::connectDataAcquisitionModel()
{
   connect(this, SIGNAL(dataChanged(DataAcquisitionDefinition)),
           dataAcquisitionForm, SLOT(handleDataChanged(DataAcquisitionDefinition)));
   connect(this, SIGNAL(daqDurationChanged(double)),
           dataAcquisitionForm, SLOT(handleDaqDurationChanged(double)));
   connect(this, SIGNAL(biasVoltageChanged(bool)),
           detectorControlForm, SLOT(handleBiasVoltageChanged(bool)));
}

void DataAcquisitionModel::connectDataAcquisitionForm()
{
   connect(dataAcquisitionForm, SIGNAL(executeCommand(GigEDetector::DetectorCommand, int)),
           gigEDetector, SLOT(handleExecuteCommand(GigEDetector::DetectorCommand, int)));
   connect(dataAcquisitionForm, SIGNAL(dataFilenameChanged(DetectorFilename)),
           this, SLOT(handleDataFilenameChanged(DetectorFilename)));
   connect(dataAcquisitionForm, SIGNAL(logFilenameChanged(DetectorFilename)),
           this, SLOT(handleLogFilenameChanged(DetectorFilename)));
   connect(dataAcquisitionForm, SIGNAL(dataAcquisitionDefinitionChanged(DataAcquisitionDefinition)),
           this, SLOT(handleDataAcquisitionDefinitionChanged(DataAcquisitionDefinition)));
   connect(dataAcquisitionForm, SIGNAL(collectImagesPressed()),
           dataAcquisition, SLOT(handleCollectReducedImages()));
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
   connect(dataAcquisitionForm, SIGNAL(saveRawChanged(bool)),
           gigEDetector, SLOT(handleSaveRawChanged(bool)));
}

void DataAcquisitionModel::connectHV()
{
   connect(hv, SIGNAL(prepareForBiasRefresh()), dataAcquisition, SLOT(prepareForBiasRefresh()));

   connect(hv, SIGNAL(biasRefreshing()), detectorMonitor, SLOT(handleBiasRefreshing()));
   connect(hv, SIGNAL(biasRefreshing()), dataAcquisition, SLOT(handleBiasRefreshing()));

   connect(hv, SIGNAL(biasRefreshed(QString, bool)), dataAcquisitionForm, SLOT(handleBiasRefreshed(QString, bool)));
   connect(hv, SIGNAL(biasRefreshed(QString, bool)), detectorControlForm, SLOT(handleBiasRefreshed(QString, bool)));
   connect(hv, SIGNAL(biasRefreshed(QString, bool)), dataAcquisition, SLOT(handleBiasRefreshed(QString, bool)));

   connect(hv, SIGNAL(biasState(bool)), dataAcquisition, SLOT(handleBiasState(bool)));
   connect(hv, SIGNAL(biasVoltageChanged(bool)), this, SLOT(handleBiasVoltageChanged(bool)));

   connect(hv, SIGNAL(setHV(double)), gigEDetector, SLOT(handleSetHV(double)));
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

void DataAcquisitionModel::configure(QString directory, QString prefix, bool timestampOn, bool offsets,
                                     double duration, int repeatCount, int repeatInterval,
                                     bool triggering, int ttlInput)
{
   DetectorFilename *dataFilename = dataAcquisitionDefinition.getDataFilename();
   dataFilename->setDirectory(directory);
   dataFilename->setPrefix(prefix);
   dataFilename->setTimestampOn(timestampOn);
   dataAcquisitionDefinition.setDuration(duration * 1000);
   dataAcquisitionDefinition.setRepeatCount(repeatCount);
   dataAcquisitionDefinition.setRepeatInterval(repeatInterval * 1000);
   dataAcquisitionDefinition.setOffsets(offsets);
   dataAcquisitionDefinition.setTriggering(triggering);
   dataAcquisitionDefinition.setTtlInput(ttlInput);

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
   dataAcquisitionDefinition.setDataFilename(dataFilename);
}
/*
void DataAcquisitionModel::handleTriggeringChanged(DetectorFilename dataFilename)
{
   setDetectorFilename(dataFilename, dataAcquisitionDefinition.getDataFilename());
   dataAcquisitionDefinition.setDataFilename(dataFilename);
}
*/
void DataAcquisitionModel::handleLogFilenameChanged(DetectorFilename logFilename)
{
   setDetectorFilename(logFilename, dataAcquisitionDefinition.getLogFilename());
   dataAcquisitionDefinition.setLogFilename(logFilename);
}

void DataAcquisitionModel::changeDaqDuration()
{
   int splitDataCollections;
   int repeatCount = dataAcquisitionDefinition.getRepeatCount();
   int repeatInterval = dataAcquisitionDefinition.getRepeatInterval();
   int biasRefreshTime = hv->getBiasRefreshTime();
   bool biasOn = hv->getBiasOnState();

   daqDuration = dataAcquisitionDefinition.getDuration();
   daqCollectionDuration = repeatCount * daqDuration;
   splitDataCollections = ceil(((double) daqDuration) / ((double ) hv->getBiasRefreshInterval()));

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
      daqDuration -= hv->getBiasRefreshTime();
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
   this->dataAcquisitionDefinition.setTriggering(dataAcquisitionDefinition.isTriggering());
   this->dataAcquisitionDefinition.setTtlInput(dataAcquisitionDefinition.getTtlInput());

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

void DataAcquisitionModel::handleTriggeringChanged(int triggering)
{
   if (triggering != 0)
   {
      this->dataAcquisitionDefinition.setTriggering(true);
   }
   else
   {
      this->dataAcquisitionDefinition.setTriggering(false);
   }
   emit dataChanged(dataAcquisitionDefinition);
}
