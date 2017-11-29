#include <QDebug>
#include <QFileDialog>

#include "dataacquisitionform.h"
#include "ui_dataacquisitionform.h"

DataAcquisitionForm::DataAcquisitionForm(QWidget *parent) :
   QWidget(parent),
   ui(new Ui::DataAcquisitionForm)
{
   ui->setupUi(this);

   mainWindow = new QMainWindow();
   mainWindow->setCentralWidget(this);

   invalidItemText = QString("Please Select");

   dataFilename = dataAcquisitionDefinition.getDataFilename();
   logFilename = dataAcquisitionDefinition.getLogFilename();

   offsetsDialog = new OffsetsDialog(this);
   dataCollectionDialog = new DataCollectionDialog(this);
   operatedForScripting = false;
   loggingEnabled = false;
   ui->duration->setSingleStep(0.01);

   connectSignals();
   handleSaveRawChanged(true);
//   handleDataAcquisitionDefinition();
}

DataAcquisitionForm::~DataAcquisitionForm()
{
   delete ui;
}

QMainWindow *DataAcquisitionForm::getMainWindow()
{
   return mainWindow;
}

void DataAcquisitionForm::connectSignals()
{
   connect(ui->collectImages, SIGNAL(pressed()), this, SLOT(handleCollectImagesPressed()));
   connect(ui->abortDAQ, SIGNAL(pressed()), this, SLOT(handleAbortDAQPressed()));
   connect(ui->saveRaw, SIGNAL(toggled(bool)), this, SLOT(handleSaveRawChanged(bool)));
   connect(ui->dataFileTimestamp, SIGNAL(stateChanged(int)), this, SLOT(handleDataFilename()));
   connect(ui->dataFileDirectory, SIGNAL(textChanged(QString)), this, SLOT(handleDataFilename()));
   connect(ui->dataFilePrefix, SIGNAL(textChanged(QString)), this, SLOT(handleDataFilename()));
   connect(ui->dataFileDirectoryButton, SIGNAL(clicked()), this, SLOT(setDataDirectory()));
   connect(ui->logFileTimestamp, SIGNAL(stateChanged(int)), this, SLOT(handleLogFilename()));
   connect(ui->logFileDirectory, SIGNAL(textChanged(QString)), this, SLOT(handleLogFilename()));
   connect(ui->logFilePrefix, SIGNAL(textChanged(QString)), this, SLOT(handleLogFilename()));
   connect(ui->logFileDirectoryButton, SIGNAL(clicked()), this, SLOT(setLogDirectory()));
   connect(ui->loggingEnabled, SIGNAL(stateChanged(int)), this, SLOT(handleLoggingEnabled(int)));
   connect(ui->offsetsButton, SIGNAL(stateChanged(int)), this, SLOT(handleDataAcquisitionDefinition()));
   connect(ui->duration, SIGNAL(valueChanged(double)), this, SLOT(handleDataAcquisitionDefinition()));
   connect(ui->repeatInterval, SIGNAL(valueChanged(double)), this, SLOT(handleDataAcquisitionDefinition()));
   connect(ui->repeatCount, SIGNAL(valueChanged(int)), this, SLOT(handleDataAcquisitionDefinition()));
   connect(offsetsDialog, SIGNAL(accepted()), this, SLOT(offsetsDialogAccepted()));
   connect(offsetsDialog, SIGNAL(rejected()), this, SLOT(offsetsDialogRejected()));
   connect(dataCollectionDialog, SIGNAL(accepted()), this, SLOT(dataCollectionDialogAccepted()));
   connect(dataCollectionDialog, SIGNAL(rejected()), this, SLOT(dataCollectionDialogRejected()));
}

void DataAcquisitionForm::handleDaqDurationChanged(double daqDuration)
{
   long daqDurationSeconds = floor(daqDuration);
   long days = daqDurationSeconds / seconds_in_day;
   long hundredths = floor((daqDuration - daqDurationSeconds) * 100 + 0.5);
   QString daqDurationTime = "";
   if (days > 0)
   {
       daqDurationTime = QString::number(days) +"d ";
   }

   daqDurationTime += QTime(0,0).addSecs(daqDurationSeconds).toString("hh:mm:ss");;
   QString hundredthsStr;

   this->daqDuration = daqDuration;
   hundredthsStr = QString::number(hundredths);
   hundredthsStr = prepend0(hundredthsStr, 2);

   daqDurationTime += "." +
         hundredthsStr;

   ui->estimatedDuration->setText(daqDurationTime);
}

void DataAcquisitionForm::handleControlledByGui()
{
   operatedForScripting = true;
   disableGui();
}

void DataAcquisitionForm::handleScriptRelease(QString name)
{
      operatedForScripting = false;
      guiReady();
}

QString DataAcquisitionForm::prepend0(QString input, int number)
{
   int leadingZeros;

   leadingZeros = number - input.length();
   for (int i = 0; i < leadingZeros; i++)
   {
      input.prepend('0');
   }
   return input;
}

void DataAcquisitionForm::setDaqName(QString daqName)
{
   this->daqName = daqName;
}

void DataAcquisitionForm::setDaqModelName(QString daqModelName)
{
   this->daqModelName = daqModelName;
}

void DataAcquisitionForm::handleCollectImagesPressed()
{
   /*
   try
   {
      emit collectImagesPressed();
   }
   catch (DetectorException &ex)
   {
      qDebug() << "handleCollectImages caught a DetectorException";
//      emit writeError(ex.getMessage());
   }
   */
   emit collectImagesPressed();
}

void DataAcquisitionForm::handleAbortDAQPressed()
{
   emit abortDAQPressed();
}

void DataAcquisitionForm::handleLoggingEnabled(int loggingEnabled)
{
   this->loggingEnabled = loggingEnabled;

   if (loggingEnabled)
   {
      enableLogfileParameters(false);
      emit createLogFile(logFilename);
   }
   else
   {
      enableLogfileParameters(true);
      emit createLogFile(NULL);
   }
}

void DataAcquisitionForm::enableLogfileParameters(bool enabled)
{
   ui->logFileDirectory->setEnabled(enabled);
   ui->logFileDirectoryButton->setEnabled(enabled);
   ui->logFilePrefix->setEnabled(enabled);
   ui->logFileTimestamp->setEnabled(enabled);
}

void DataAcquisitionForm::handleDataFilename()
{
   dataFilename->setTimestampOn(ui->dataFileTimestamp->isChecked());
   dataFilename->setDirectory(ui->dataFileDirectory->text());
   dataFilename->setPrefix(ui->dataFilePrefix->text());
   emit dataFilenameChanged(*dataFilename);
}

void DataAcquisitionForm::handleLogFilename()
{
   logFilename->setTimestampOn(ui->logFileTimestamp->isChecked());
   logFilename->setDirectory(ui->logFileDirectory->text());
   logFilename->setPrefix(ui->logFilePrefix->text());
   emit logFilenameChanged(*logFilename);
}

void DataAcquisitionForm::handleDataAcquisitionDefinition()
{
   int microSeconds = ui->duration->value() * 1000;

   dataAcquisitionDefinition.setOffsets(ui->offsetsButton->isChecked());
   dataAcquisitionDefinition.setDuration(microSeconds);
   dataAcquisitionDefinition.setRepeatInterval(ui->repeatInterval->value() * 1000.0);
   dataAcquisitionDefinition.setRepeatCount(ui->repeatCount->value());

   enableRepeats();
   emit dataAcquisitionDefinitionChanged(dataAcquisitionDefinition);
}

void DataAcquisitionForm::setDataDirectory()
{
   /* Gets a valid fileName from the user and updates the GUI.
    */
   QString destination = QFileDialog::getExistingDirectory(this, tr("Open Directory"), dataFilename->getDirectory(), QFileDialog::ShowDirsOnly);
   ui->dataFileDirectory->setText(destination);
}

void DataAcquisitionForm::setLogDirectory()
{
   /* Gets a valid fileName from the user and updates the GUI.
    */
   QString destination = QFileDialog::getExistingDirectory(this, tr("Open Directory"), logFilename->getDirectory(), QFileDialog::ShowDirsOnly);
   ui->logFileDirectory->setText(destination);
}

void DataAcquisitionForm::handleSaveRawChanged(bool saveRaw)
{
   emit saveRawChanged(saveRaw);
}

void DataAcquisitionForm::handleMonitorData(MonitorData *md)
{
   if (md->getValid())
   {
      ui->housingTemperature->setText(QString::number(md->getTH(), 'f', 1));
      ui->housingHumidity->setText(QString::number(md->getRH(), 'f', 1));
      ui->fingerTemperature->setText(QString::number(md->getT(), 'f', 1));
      ui->dewPoint->setText(QString::number(md->getTDP(), 'f', 1));
      ui->detectorTemperature->setText(QString::number(md->getTASIC(), 'f', 1));
   }
   else
   {
      disableGui();
   }
}

void DataAcquisitionForm::handleDataChanged(DataAcquisitionDefinition dataAcquisitionDefinition)
{
   QString directory = dataAcquisitionDefinition.getDataFilename()->getDirectory();
   QString prefix = dataAcquisitionDefinition.getDataFilename()->getPrefix();
   QString logDirectory = dataAcquisitionDefinition.getLogFilename()->getDirectory();
   QString logPrefix = dataAcquisitionDefinition.getLogFilename()->getPrefix();
   bool timestampOn = dataAcquisitionDefinition.getDataFilename()->getTimestampOn();

   dataFilename->setDirectory(directory);
   ui->dataFileDirectory->setText(directory);
   dataFilename->setPrefix(prefix);
   ui->dataFilePrefix->setText(prefix);
   dataFilename->setTimestampOn(timestampOn);
   ui->dataFileTimestamp->setChecked(timestampOn);
   logFilename->setDirectory(logDirectory);
   ui->logFileDirectory->setText(logDirectory);
   logFilename->setPrefix(logPrefix);
   ui->logFilePrefix->setText(logPrefix);
   logFilename->setTimestampOn(timestampOn);
   ui->logFileTimestamp->setChecked(timestampOn);

   ui->duration->setValue(dataAcquisitionDefinition.getDuration()/1000);
   ui->repeatCount->setValue(dataAcquisitionDefinition.getRepeatCount());
   ui->repeatInterval->setValue(dataAcquisitionDefinition.getRepeatInterval()/1000);
}

void DataAcquisitionForm::prepareForOffsets()
{
   // Pop up window and wait for user acknowledgement...
   offsetsDialog->show();
}

void DataAcquisitionForm::prepareForDataCollection()
{
   // Pop up window and wait for user acknowledgement...
   dataCollectionDialog->show();
}

void DataAcquisitionForm::offsetsDialogAccepted()
{
   emit executeOffsets();
}

void DataAcquisitionForm::offsetsDialogRejected()
{
   emit cancelOffsets();
   // TOD DO : replace with a writeMessage
}

void DataAcquisitionForm::dataCollectionDialogAccepted()
{
   emit executeReducedDataCollection();
}

void DataAcquisitionForm::dataCollectionDialogRejected()
{
   emit cancelReducedDataCollection();
}

void DataAcquisitionForm::guiBiasRefreshing()
{
   ui->collectImages->setEnabled(false); // Disable data collection during bias refresh
}

void DataAcquisitionForm::handleBiasRefreshed(QString time, bool restartMonitoring)
{
   ui->biasLastRefreshed->setText(time);
}

void DataAcquisitionForm::handleCollectingChanged(bool collectingOn)
{
   if (!collectingOn)
   {
      guiReady();
   }
}

void DataAcquisitionForm::handleDataAcquisitionStatusChanged(DataAcquisitionStatus status)
{
   int progress = 0;
   int currImg = status.getCurrentImage();
   int daqImgs = status.getDaqImages();

   ui->state->setText(status.getMessage());
   emit newDataAcquisitionState(status.getMessage());

   switch (status.getMajorStatus())
   {
   case DataAcquisitionStatus::IDLE:
      switch (status.getMinorStatus())
      {
         case DataAcquisitionStatus::READY:
           if (!operatedForScripting)
            {
               guiReady();
            }
            break;
         case DataAcquisitionStatus::BIAS_REFRESHING:
            if (!operatedForScripting)
            {
               guiBiasRefreshing();
            }
            break;
         case DataAcquisitionStatus::NOT_INITIALIZED:
            if (!operatedForScripting)
            {
               guiIdle();
            }
      }
   case DataAcquisitionStatus::INITIALISING:
      switch (status.getMinorStatus())
      {
      case DataAcquisitionStatus::ACTIVE:
         if (!operatedForScripting)
         {
            guiInitialising();
         }
         break;
      case DataAcquisitionStatus::READY:
//         operatedForScripting = false;
//         guiReady();
         if (!operatedForScripting)
         {
            guiReady();
         }
         break;
      case DataAcquisitionStatus::BIAS_REFRESHING:
         if (!operatedForScripting)
         {
           guiBiasRefreshing();
         }
         break;
      }
      break;
   case DataAcquisitionStatus::ACQUIRING_DATA:
      if (daqImgs > 0)
      {
         if (currImg <= daqImgs)
         {
            progress = 100 * status.getCurrentImage() / status.getDaqImages();
         }
         else
         {
            progress = 100;
         }
      }
      ui->progressBar->setValue(progress);
      ui->imageProgressBar->setValue(status.getPercentage());

      emit newDataAcquisitionProgressBarValue(progress);
      emit newDataAcquisitionImageProgressValue(status.getPercentage());

      switch (status.getMinorStatus())
      {
      case DataAcquisitionStatus::OFFSETS_PREP:
         if (!operatedForScripting)
         {
            guiOffsetsPrep();
         }
         break;
      case DataAcquisitionStatus::OFFSETS:
         if (!operatedForScripting)
         {
            guiOffsets();
         }
         break;
      case DataAcquisitionStatus::COLLECTING_PREP:
         if (!operatedForScripting)
         {
            guiCollectingPrep();
         }
         break;
      case DataAcquisitionStatus::WAITING_DARK:
         if (!operatedForScripting)
         {
            guiWaitingDarks();
         }
         break;
      case DataAcquisitionStatus::WAITING_TRIGGER:
            guiWaitingTriggers();
            break;
      case DataAcquisitionStatus::COLLECTING:
         guiCollecting();
         break;
      case DataAcquisitionStatus::DONE:
         if (!operatedForScripting)
         {
            guiReady();
         }
         break;
      case DataAcquisitionStatus::BIAS_REFRESHING:
         if (!operatedForScripting)
         {
            guiBiasRefreshing();
         }
         break;
      }
      break;
   }
}

void DataAcquisitionForm::handleTriggeringSelectionChanged(int triggering)
{
   dataAcquisitionDefinition.setTriggering(triggering);
   emit dataAcquisitionDefinitionChanged(dataAcquisitionDefinition);
}

void DataAcquisitionForm::handleTtlInputSelectionChanged(int ttlInput)
{
   dataAcquisitionDefinition.setTtlInput(ttlInput);
   emit dataAcquisitionDefinitionChanged(dataAcquisitionDefinition);
}

void DataAcquisitionForm::guiInitialising()
{
   ui->collectImages->setEnabled(false);
   ui->abortDAQ->setEnabled(false);
}

void DataAcquisitionForm::guiIdle()
{
   ui->abortDAQ->setEnabled(false);
   ui->collectImages->setEnabled(false);
   emit disableMainWindowActions();
}

void DataAcquisitionForm::guiReady()
{
   emit enableMainWindowActions();
   guiMode();
   enableRepeats();
   enableLogfileParameters(!loggingEnabled);
}

void DataAcquisitionForm::guiWaitingDarks()
{
   guiDetectorBusy();
}

void DataAcquisitionForm::guiWaitingTriggers()
{
   emit disableMainWindowActions();
   emit disableStopDAQAction();
   ui->collectImages->setEnabled(false);
   ui->abortDAQ->setEnabled(false);

   ui->collectImages->setEnabled(false);
   ui->offsetsButton->setEnabled(false);
   ui->saveRaw->setEnabled(false);
   ui->dataFileDirectory->setEnabled(false);
   ui->dataFilePrefix->setEnabled(false);
   ui->dataFileDirectoryButton->setEnabled(false);
   ui->dataFileTimestamp->setEnabled(false);
   ui->loggingEnabled->setEnabled(false);
   ui->logFileDirectoryButton->setEnabled(false);
   ui->logFileTimestamp->setEnabled(false);
   ui->abortDAQ->setEnabled(false);
   ui->duration->setEnabled(false);
   disableRepeats();
}

void DataAcquisitionForm::guiOffsets()
{
   guiDetectorBusy();
}

void DataAcquisitionForm::guiOffsetsPrep()
{
   guiDetectorBusy();
}

void DataAcquisitionForm::guiCollectingPrep()
{
   guiDetectorBusy();
}

void DataAcquisitionForm::guiCollecting()
{
   guiDetectorBusy();
   ui->abortDAQ->setEnabled(true);
}

void DataAcquisitionForm::guiDetectorBusy()
{
   emit disableMainWindowActions();
   ui->collectImages->setEnabled(false);
   ui->abortDAQ->setEnabled(false);

   ui->collectImages->setEnabled(false);
   ui->offsetsButton->setEnabled(false);
   ui->saveRaw->setEnabled(false);
   ui->dataFileDirectory->setEnabled(false);
   ui->dataFilePrefix->setEnabled(false);
   ui->dataFileDirectoryButton->setEnabled(false);
   ui->dataFileTimestamp->setEnabled(false);
   ui->loggingEnabled->setEnabled(false);
   ui->logFileDirectoryButton->setEnabled(false);
   ui->logFileTimestamp->setEnabled(false);
   ui->abortDAQ->setEnabled(false);
   ui->duration->setEnabled(false);
   disableRepeats();
}

void DataAcquisitionForm::disableGui()
{
   guiDetectorBusy();
   ui->abortDAQ->setEnabled(false);
}

void DataAcquisitionForm::guiMode()
{
   ui->abortDAQ->setEnabled(false);
   ui->loggingEnabled->setEnabled(true);
   disableRepeats();

   int mode = GigEDetector::CONTINUOUS;
   switch (mode)
   {
   case GigEDetector::CONTINUOUS:
      ui->collectImages->setEnabled(true);
      ui->duration->setEnabled(true);
      ui->dataFileDirectory->setEnabled(true);
      ui->dataFileDirectoryButton->setEnabled(true);
      ui->dataFilePrefix->setEnabled(true);
      ui->dataFileTimestamp->setEnabled(true);
      ui->offsetsButton->setEnabled(true);
      ui->saveRaw->setEnabled(true);
      enableRepeats();
      break;

   case GigEDetector::GIGE_DEFAULT:
   case GigEDetector::INVALID_MODE:
      ui->collectImages->setEnabled(false);
      ui->duration->setEnabled(false);
      ui->dataFileDirectory->setEnabled(false);
      ui->dataFileDirectoryButton->setEnabled(false);
      ui->dataFilePrefix->setEnabled(false);
      ui->dataFileTimestamp->setEnabled(false);
      ui->offsetsButton->setEnabled(false);
      ui->saveRaw->setEnabled(false);
      break;
   }
}

void DataAcquisitionForm::enableRepeats()
{
   ui->repeatCount->setEnabled(true);
   if (ui->repeatCount->value() > 1)
   {
      ui->repeatInterval->setEnabled(true);
   }
   else
   {
      ui->repeatInterval->setEnabled(false);
   }
}

void DataAcquisitionForm::disableRepeats()
{
   ui->repeatCount->setEnabled(false);
   ui->repeatInterval->setEnabled(false);
}

