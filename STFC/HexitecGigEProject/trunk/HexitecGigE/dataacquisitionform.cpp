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
   handleDataAcquisitionDefinition();
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
   connect(ui->mode, SIGNAL(currentIndexChanged(int)), this, SLOT(modeChanged(int)));
   connect(ui->collectImages, SIGNAL(pressed()), this, SLOT(handleCollectImagesPressed()));
   connect(ui->initTrigger, SIGNAL(pressed()), this, SLOT(handleInitTriggerPressed()));
   connect(ui->trigger, SIGNAL(pressed()), this, SLOT(handleTriggerPressed()));
   connect(ui->stopTrigger, SIGNAL(pressed()), this, SLOT(handleStopTriggerPressed()));
   connect(ui->abortDAQ, SIGNAL(pressed()), this, SLOT(handleAbortDAQPressed()));
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
   //qDebug() << "Scripting wants GUI to control script.";
   disableGui();
}

void DataAcquisitionForm::handleScriptRelease(QString name)
{
      //qDebug() <<"Handling script release of " << name;
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
   emit collectImagesPressed();
}

void DataAcquisitionForm::handleInitTriggerPressed()
{
   emit initTriggerPressed();
}

void DataAcquisitionForm::handleTriggerPressed()
{
   emit triggerPressed();
}

void DataAcquisitionForm::handleStopTriggerPressed()
{
   emit stopTriggerPressed();
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
   enableRepeats(ui->mode->currentIndex());
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

void DataAcquisitionForm::handleMonitorData(MonitorData *md)
{
   ui->housingTemperature->setText(QString::number(md->getTH(), 'f', 1));
   ui->housingHumidity->setText(QString::number(md->getRH(), 'f', 1));
   ui->fingerTemperature->setText(QString::number(md->getT(), 'f', 1));
   ui->dewPoint->setText(QString::number(md->getTDP(), 'f', 1));
   ui->biasCurrent->setText(QString::number(md->getIK(), 'g', 3));
   ui->detectorTemperature->setText(QString::number(md->getTASIC(), 'f', 1));
}

void DataAcquisitionForm::setModes(QStringList modes)
{
   ui->mode->addItems(modes);
   ui->mode->addItem(invalidItemText);
}

void DataAcquisitionForm::modeChanged(int mode)
{
   //qDebug() << "DataAcquisitionForm::modeChanged(int mode): " << mode;
   ui->mode->setCurrentIndex(mode);
   if (ui->mode->currentText() != invalidItemText)
   {
       qDebug() <<"Change GUI sensitivity here!";
//      emit executeCommand(GigEDetector::CONFIGURE, mode);
   }
}

void DataAcquisitionForm::handleModeChanged(GigEDetector::Mode mode)
{
   ui->mode->setCurrentIndex(mode);
}

void DataAcquisitionForm::handleDataChanged(DataAcquisitionDefinition dataAcquisitionDefinition)
{
   QString directory = dataAcquisitionDefinition.getDataFilename()->getDirectory();
   QString prefix = dataAcquisitionDefinition.getDataFilename()->getPrefix();
   bool timestampOn = dataAcquisitionDefinition.getDataFilename()->getTimestampOn();

   dataFilename->setDirectory(directory);
   ui->dataFileDirectory->setText(directory);
   dataFilename->setPrefix(prefix);
   ui->dataFilePrefix->setText(prefix);
   dataFilename->setTimestampOn(timestampOn);
   ui->dataFileTimestamp->setChecked(timestampOn);
   logFilename->setDirectory(directory);
   ui->logFileDirectory->setText(directory);
   logFilename->setPrefix(prefix);
   ui->logFilePrefix->setText(prefix);
   logFilename->setTimestampOn(timestampOn);
   ui->logFileTimestamp->setChecked(timestampOn);

   ui->duration->setValue(dataAcquisitionDefinition.getDuration()/1000);
   ui->repeatCount->setValue(dataAcquisitionDefinition.getRepeatCount());
   ui->repeatInterval->setValue(dataAcquisitionDefinition.getRepeatInterval()/1000);
}

void DataAcquisitionForm::handleDataChanged(QString mode)
{
   //qDebug() <<"Scripting causing handleDataChanged with mode: " << mode;
   int count = ui->mode->count();

   for (int i = 0; i < count; i++)
   {
      if (mode == ui->mode->itemText(i))
      {
         if (mode != ui->mode->currentText())
         {
            operatedForScripting = true;
            //qDebug() << "Scripting wants GUI to control mode change.";
            disableGui();
            ui->mode->setCurrentIndex(i);
         }
         break;
      }
   }
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
   ui->trigger->setEnabled(false); // Disable triggering during bias refresh
   ui->initTrigger->setEnabled(false); // Disable initialise trigger during bias refresh
}

void DataAcquisitionForm::handleBiasRefreshed(QString time)
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
   ui->state->setText(status.getMessage());
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
//         qDebug() << "status changed to INITIALISING:READY ";
//         operatedForScripting = false;
//         guiReady();
         if (!operatedForScripting)
         {
            //qDebug() << "status changed to INITIALISING:READY ";
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
      int progress = 100;
      int currImg = status.getCurrentImage();
      int daqImgs = status.getDaqImages();
      if (currImg > 0 && daqImgs > 0)
      {
         if (currImg <= daqImgs)
         {
            progress = 100 * status.getCurrentImage() / status.getDaqImages();
         }
      }
      ui->progressBar->setValue(progress);
      ui->imageProgressBar->setValue(status.getPercentage());
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
      case DataAcquisitionStatus::COLLECTING:
         guiCollecting();
         break;
      case DataAcquisitionStatus::WAITING_TRIGGER:
         if (!operatedForScripting)
         {
            guiWaitingTrigger();
         }
         break;
      case DataAcquisitionStatus::TRIGGERING_STOPPED:
         if (!operatedForScripting)
         {
            guiReady();
         }
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

void DataAcquisitionForm::guiInitialising()
{
   ui->collectImages->setEnabled(false);
   ui->abortDAQ->setEnabled(false);
   ui->initTrigger->setEnabled(false);
}

void DataAcquisitionForm::guiIdle()
{
   ui->abortDAQ->setEnabled(false);
}

void DataAcquisitionForm::guiReady()
{
   int mode = ui->mode->currentIndex();
   guiMode(ui->mode->currentIndex());
   //qDebug() << "DataAcquisitionForm::guiReady() mode from comboBox: " << mode;
   enableRepeats(mode);
   enableLogfileParameters(!loggingEnabled);
}

void DataAcquisitionForm::guiWaitingDarks()
{
   guiDetectorBusy();
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
   ui->trigger->setEnabled(false);
   ui->stopTrigger->setEnabled(false);
}

void DataAcquisitionForm::guiWaitingTrigger()
{
   int mode = ui->mode->currentIndex();
   guiDetectorBusy();

   if (mode == GigEDetector::SOFT_TRIGGER)
   {
       ui->trigger->setEnabled(true);
   }
   else
   {
       ui->trigger->setEnabled(false);
   }

   ui->stopTrigger->setEnabled(true);
   ui->initTrigger->setEnabled(false);
}

void DataAcquisitionForm::guiDetectorBusy()
{
   ui->collectImages->setEnabled(false);
   ui->abortDAQ->setEnabled(false);

   ui->mode->setEnabled(false);
   ui->collectImages->setEnabled(false);
   ui->offsetsButton->setEnabled(false);
   ui->dataFileDirectory->setEnabled(false);
   ui->dataFilePrefix->setEnabled(false);
   ui->dataFileDirectoryButton->setEnabled(false);
   ui->dataFileTimestamp->setEnabled(false);
   ui->loggingEnabled->setEnabled(false);
   ui->logFileDirectoryButton->setEnabled(false);
   ui->logFileTimestamp->setEnabled(false);
   ui->abortDAQ->setEnabled(false);
   ui->duration->setEnabled(false);
   ui->trigger->setEnabled(false);
   ui->initTrigger->setEnabled(false);
   disableRepeats();
}

void DataAcquisitionForm::disableGui()
{
   guiDetectorBusy();
   ui->abortDAQ->setEnabled(false);
}

void DataAcquisitionForm::guiMode(int mode)
{
   ui->abortDAQ->setEnabled(false);
   ui->trigger->setEnabled(false);
   ui->mode->setEnabled(true);
   ui->loggingEnabled->setEnabled(true);
   disableRepeats();

   mode = GigEDetector::CONTINUOUS;
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
      ui->initTrigger->setEnabled(false);
      ui->stopTrigger->setEnabled(false);
      enableRepeats(mode);
      break;
   case GigEDetector::SOFT_TRIGGER:
      ui->collectImages->setEnabled(false);
      ui->duration->setEnabled(true);
      ui->dataFileDirectory->setEnabled(true);
      ui->dataFileDirectoryButton->setEnabled(true);
      ui->dataFilePrefix->setEnabled(true);
      ui->dataFileTimestamp->setEnabled(true);
      ui->offsetsButton->setEnabled(true);
      ui->initTrigger->setEnabled(true);
      ui->stopTrigger->setEnabled(false);
      break;
   case GigEDetector::EXTERNAL_TRIGGER:
      ui->collectImages->setEnabled(false);
      ui->duration->setEnabled(true);
      ui->dataFileDirectory->setEnabled(true);
      ui->dataFileDirectoryButton->setEnabled(true);
      ui->dataFilePrefix->setEnabled(true);
      ui->dataFileTimestamp->setEnabled(true);
      ui->offsetsButton->setEnabled(true);
      ui->initTrigger->setEnabled(true);
      ui->stopTrigger->setEnabled(false);
      break;
   case GigEDetector::FIXED:
   case GigEDetector::GIGE_DEFAULT:
   case GigEDetector::INVALID_MODE:
      ui->collectImages->setEnabled(false);
      ui->duration->setEnabled(false);
      ui->dataFileDirectory->setEnabled(false);
      ui->dataFileDirectoryButton->setEnabled(false);
      ui->dataFilePrefix->setEnabled(false);
      ui->dataFileTimestamp->setEnabled(false);
      ui->offsetsButton->setEnabled(false);
      ui->initTrigger->setEnabled(false);
      ui->stopTrigger->setEnabled(false);
      break;
   }
}

void DataAcquisitionForm::enableRepeats(int mode)
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

