#include <QThread>
#include "parameters.h"
#include "detectorcontrolform.h"
#include "ui_detectorcontrolform.h"
#include "objectreserver.h"

DetectorControlForm::DetectorControlForm(QWidget *parent) :
   QWidget(parent),
   ui(new Ui::DetectorControlForm)
{
   waitingForModeChange = false;
   ui->setupUi(this);
   ui->logFileDirectoryLabel->setVisible(false);
   ui->logFileDirectory->setVisible(false);
   ui->logFileDirectoryButton->setVisible(false);
   ui->logFilePrefixLabel->setVisible(false);
   ui->logFilePrefix->setVisible(false);
   ui->logFileTimestamp->setVisible(false);
   ui->loggingEnabled->setVisible(false);
   ui->triggeringSelection->setEnabled(false);
   ui->ttlInputSelection->setEnabled(false);
   ui->triggerTimeout->setEnabled(false);
   ui->triggerTimeoutButton->setEnabled(false);

   mainWindow = new QMainWindow();
   mainWindow->setCentralWidget(this);
   dpWarningDialog = new DPWarningDialog(this);
   hvReservedByScripting = false;
   tAboveTdp = false;
   targetTemperature = 20.0;
   targetTemperatureMin = 18.0;
   targetTemperatureMax = 35.0;
   triggerTimeout = 60.0;

   readIniFile();
   ui->setFingerTemperature->setMinimum(targetTemperatureMin);
   ui->setFingerTemperature->setMaximum(targetTemperatureMax);
   ui->setFingerTemperature->setValue(targetTemperature);
   firstMonitor = true;

   connectSignals();
   ui->triggerTimeout->setValue(triggerTimeout);
   guiReady();
}

void DetectorControlForm::readIniFile()
{
   QString detectorFilename = Parameters::twoEasyIniFilename;
   QSettings settings(QSettings::UserScope, "TEDDI", "HexitecGigE");
   double targetTemperature;
   double targetTemperatureMin;
   double targetTemperatureMax;
   double triggerTimeout;

   if (settings.contains("hexitecGigEIniFilename"))
   {
      detectorFilename = settings.value("hexitecGigEIniFilename").toString();
   }
   IniFile *detectorIniFile = new IniFile(detectorFilename);

   if ((targetTemperature = detectorIniFile->getDouble("Environmental/Target_Temperature")) != QVariant(INVALID))
   {
      this->targetTemperature = targetTemperature;
   }
   if ((targetTemperatureMin = detectorIniFile->getDouble("Environmental/Target_Temperature_Min")) != QVariant(INVALID))
   {
      this->targetTemperatureMin = targetTemperatureMin;
   }
   if ((targetTemperatureMax = detectorIniFile->getDouble("Environmental/Target_Temperature_Max")) != QVariant(INVALID))
   {
      this->targetTemperatureMax = targetTemperatureMax;
   }
   if ((triggerTimeout = detectorIniFile->getDouble("Environmental/Trigger_Timeout")) != QVariant(INVALID))
   {
      this->triggerTimeout = triggerTimeout;
   }

   return;
}

DetectorControlForm::~DetectorControlForm()
{
   delete ui;
}

QMainWindow *DetectorControlForm::getMainWindow()
{
   return mainWindow;
}

void DetectorControlForm::setHvName(QString hvName)
{
   this->hvName = hvName;
}

void DetectorControlForm::handleTriggeringAvailable(bool triggeringAvailable)
{
   this->triggeringAvailable = triggeringAvailable;

   if (this->triggeringAvailable)
   {
      ui->triggeringSelection->setEnabled(true);
      ui->ttlInputSelection->setEnabled(true);
      ui->triggerTimeout->setEnabled(true);
      ui->triggerTimeoutButton->setEnabled(true);
   }
   else
   {
      ui->triggeringSelection->setEnabled(false);
      ui->ttlInputSelection->setEnabled(false);
      ui->triggerTimeout->setEnabled(false);
      ui->triggerTimeoutButton->setEnabled(false);
   }
}

void DetectorControlForm::connectSignals()
{
   connect(ui->biasVoltageButton, SIGNAL(clicked(bool)), this, SLOT(biasVoltageClicked(bool)));
   connect(ui->collectImages, SIGNAL(pressed()), this, SLOT(handleCollectImagesPressed()));
   connect(ui->initialiseConnection, SIGNAL(pressed()), this, SLOT(initialiseDetectorPressed()));
   connect(ui->terminateConnection, SIGNAL(pressed()), this, SLOT(terminateDetectorPressed()));
   connect(ui->abortDAQ, SIGNAL(pressed()), this, SLOT(abortDAQ()));
   connect(ui->imageCount, SIGNAL(valueChanged(int)), this, SLOT(handleFixedImageCountChanged(int)));
   connect(ui->setFingerTemperatureButton, SIGNAL(pressed()), this, SLOT(handleSetFingerTemperature()));
   connect(ui->triggeringSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(handleTriggeringSelectionChanged(int)));
   connect(ui->ttlInputSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(handleTtlInputSelectionChanged(int)));
   connect(ui->triggerTimeoutButton, SIGNAL(pressed()), this, SLOT(handleSetTriggerTimeout()));
}

void DetectorControlForm::handleCollectImagesPressed()
{
   emit collectImagesPressed();
   waitingForModeChange = true;
}

void DetectorControlForm::abortDAQ()
{
   emit abortDAQPressed();
}

void DetectorControlForm::handleFixedImageCountChanged(int fixedImageCount)
{
   emit fixedImageCountChanged(fixedImageCount);
}

void DetectorControlForm::handleSetFingerTemperature()
{
   double temperature = ui->setFingerTemperature->value();
   emit setFingerTemperature(temperature);
}

void DetectorControlForm::handleHVOn()
{
   biasVoltageClicked(true);
}

void DetectorControlForm::handleHVOff()
{
   biasVoltageClicked(false);
}

void DetectorControlForm::biasVoltageClicked(bool biasVoltageOn)
{
   if (biasVoltageOn)
   {
      emit executeCommand(HV::HVON);
   }
   else
   {
      emit executeCommand(HV::HVOFF);
   }
   emit biasVoltageChanged(biasVoltageOn);
}

void DetectorControlForm::initialiseDetectorPressed()
{
   try
   {
      emit initialiseDetector();
      QThread::sleep(2);

      /* Need to properly get status back from detector class when commnad
    * executed via signal/slot. Set GUI correctly. */
      ui->biasVoltageButton->setChecked(false);
      ui->initialiseConnection->setEnabled(false);
      ui->terminateConnection->setEnabled(true);
   }
   catch (DetectorException &ex)
   {
      initialiseFailed();
      emit writeError(ex.getMessage());
   }
}

void DetectorControlForm::terminateDetectorPressed()
{
   terminateDetector();
}

void DetectorControlForm::terminateDetector()
{
   emit disableMonitoring();
   emit disableBiasRefresh();
   emit executeCommand(GigEDetector::CLOSE, 0, 0);

   QThread::msleep(100);
   /* Need to properly get status back from detector class when commnad
    * executed via signal/slot. Set GUI correctly. */
   guiDetectorBusy();
   ui->initialiseConnection->setEnabled(true);
   ui->terminateConnection->setEnabled(false);
}

void DetectorControlForm::initialiseFailed()
{
   emit executeCommand(GigEDetector::CLOSE, 0, 0);
   guiIdle();
}

void DetectorControlForm::handleMonitorData(MonitorData *md)
{
   if (md->getValid())
   {
      ui->housingTemperature->setText(QString::number(md->getTH(), 'f', 1));
      ui->housingHumidity->setText(QString::number(md->getRH(), 'f', 1));
      ui->fingerTemperature->setText(QString::number(md->getT(), 'f', 1));
      ui->dewPoint->setText(QString::number(md->getTDP(), 'f', 1));
      ui->detectorTemperature->setText(QString::number(md->getTASIC(), 'f', 1));
      if (firstMonitor)
      {
//         ui->setFingerTemperature->setValue(md->getT());
//         qDebug() << "DetectorControlForm::handleMonitorData(MonitorData *md)" << md->getT();
         firstMonitor = false;
      }
   }
   else
   {
      terminateDetector();
   }
}

void DetectorControlForm::handleTemperatureBelowDP()
{
   tAboveTdp = false;
   ui->biasVoltageButton->setChecked(false);
   ui->biasVoltageButton->setEnabled(false);
   dpWarningDialog->show();
}

void DetectorControlForm::handleTemperatureAboveDP()
{
   tAboveTdp = true;
   if (!hvReservedByScripting)
   {
      ui->biasVoltageButton->setEnabled(true);
   }
}

void DetectorControlForm::handleCollectingChanged(bool collectingOn)
{
   if (!collectingOn)
   {
      guiReady();
   }
}

void DetectorControlForm::handleDataAcquisitionStatusChanged(DataAcquisitionStatus status)
{
   ui->state->setText(status.getMessage());
   switch (status.getMajorStatus())
   {
   case DataAcquisitionStatus::IDLE:
      switch (status.getMinorStatus())
      {
         case DataAcquisitionStatus::READY:
            guiReady();
            break;
         case DataAcquisitionStatus::BIAS_REFRESHING:
            guiBiasRefreshing();
            break;
         case DataAcquisitionStatus::NOT_INITIALIZED:
            guiIdle();
            break;
      }
   case DataAcquisitionStatus::INITIALISING:
      switch (status.getMinorStatus())
      {
      case DataAcquisitionStatus::ACTIVE:
         guiInitialising();
         break;
      case DataAcquisitionStatus::READY:
         guiReady();
         break;
      case DataAcquisitionStatus::DONE:
         guiReady();
         break;
      case DataAcquisitionStatus::BIAS_REFRESHING:
         guiBiasRefreshing();
         break;
      }
      break;
   case DataAcquisitionStatus::ACQUIRING_DATA:
      switch (status.getMinorStatus())
      {
      case DataAcquisitionStatus::OFFSETS_PREP:
         guiOffsetsPrep();
         break;
      case DataAcquisitionStatus::OFFSETS:
         guiOffsets();
         break;
      case DataAcquisitionStatus::COLLECTING_PREP:
         guiCollectingPrep();
         break;
      case DataAcquisitionStatus::WAITING_DARK:
         guiWaitingDarks();
         break;
      case DataAcquisitionStatus::COLLECTING:
         guiCollecting();
         break;
      case DataAcquisitionStatus::DONE:
         guiReady();
         break;
      case DataAcquisitionStatus::BIAS_REFRESHING:
         guiBiasRefreshing();
         break;
      }
      break;
   }
}

void DetectorControlForm::handleBiasVoltageChanged(bool biasOn)
{
   ui->biasVoltageButton->setChecked(biasOn);
}

void DetectorControlForm::handleTriggeringSelectionChanged(int triggering)
{
   emit disableMonitoring();
   emit triggeringSelectionChanged(triggering);
}

void DetectorControlForm::handleTtlInputSelectionChanged(int ttlInput)
{
   emit ttlInputSelectionChanged(ttlInput);
}


void DetectorControlForm::handleSetTriggerTimeout()
{
   double triggerTimeout = ui->triggerTimeout->value();

   emit setTriggerTimeout(triggerTimeout);
}

void DetectorControlForm::handleScriptReserve(QString name)
{
   if (name == hvName)
   {
      ui->biasVoltageButton->setEnabled(false);
      hvReservedByScripting = true;
   }
}

void DetectorControlForm::handleScriptRelease(QString name)
{
   if (name == hvName)
   {
      ui->biasVoltageButton->setEnabled(true);
      hvReservedByScripting = false;
   }
}

void DetectorControlForm::guiBiasRefreshing()
{
   ui->collectImages->setEnabled(false); // Disable data collection during bias refresh
   if (!hvReservedByScripting)
   {
      ui->biasVoltageButton->setEnabled(false); // Don't allow bias to be turned off during bias refresh
      ui->terminateConnection->setEnabled(false); // Don't allow termination during bias refresh
   }
}

void DetectorControlForm::handleBiasRefreshed(QString time, bool restartMonitoring)
{
   ui->biasLastRefreshed->setText(time);
   if (!hvReservedByScripting)
   {
      ui->biasVoltageButton->setEnabled(true); // Allow bias to be turned on/off again
      ui->terminateConnection->setEnabled(true); // Allow termination again
   }
}

void DetectorControlForm::setPixmap(QPixmap pixmap)
{
   ui->imageLabel->setPixmap(pixmap.scaled(240, 240));
}

void DetectorControlForm::guiInitialising()
{
   ui->initialiseConnection->setEnabled(false);
   ui->terminateConnection->setEnabled(false);
   ui->collectImages->setEnabled(false);
   ui->abortDAQ->setEnabled(false);
}

void DetectorControlForm::guiIdle()
{
   ui->initialiseConnection->setEnabled(true);
   ui->terminateConnection->setEnabled(false);
   ui->setFingerTemperatureButton->setEnabled(false);
   ui->abortDAQ->setEnabled(false);
   emit disableHVActions();
}

void DetectorControlForm::guiReady()
{
   ui->initialiseConnection->setEnabled(false);
   ui->terminateConnection->setEnabled(true);
   ui->collectImages->setEnabled(true);
   ui->abortDAQ->setEnabled(false);
   ui->imageCount->setEnabled(true);
   ui->setFingerTemperatureButton->setEnabled(true);
   ui->triggeringSelection->setEnabled(true);
   ui->ttlInputSelection->setEnabled(true);

   if (ui->triggeringSelection->currentIndex() == 0)
   {
      ui->collectImages->setEnabled(true);
   }
   else
   {
      ui->collectImages->setEnabled(false);
   }

   if (!hvReservedByScripting && tAboveTdp)
   {
      ui->biasVoltageButton->setEnabled(true);
   }
   emit enableHVActions();
}

void DetectorControlForm::guiWaitingDarks()
{
   guiDetectorBusy();
}

void DetectorControlForm::guiOffsets()
{
   guiDetectorBusy();
}

void DetectorControlForm::guiOffsetsPrep()
{
   guiDetectorBusy();
}

void DetectorControlForm::guiCollectingPrep()
{
   guiDetectorBusy();
}

void DetectorControlForm::guiCollecting()
{
   guiDetectorBusy();
   ui->abortDAQ->setEnabled(true);
}

void DetectorControlForm::guiDetectorBusy()
{
   emit disableHVActions();
   ui->initialiseConnection->setEnabled(false);
   ui->terminateConnection->setEnabled(false);
   ui->imageCount->setEnabled(false);
   ui->collectImages->setEnabled(false);
   ui->abortDAQ->setEnabled(false);
   ui->biasVoltageButton->setEnabled(false);
   ui->triggeringSelection->setEnabled(false);
   ui->ttlInputSelection->setEnabled(false);
}
