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

   mainWindow = new QMainWindow();
   mainWindow->setCentralWidget(this);
   dpWarningDialog = new DPWarningDialog(this);
   keithleyReservedByScripting = false;
   tAboveTdp = false;
   firstMonitor = true;


   connectSignals();
}

DetectorControlForm::~DetectorControlForm()
{
   delete ui;
}

QMainWindow *DetectorControlForm::getMainWindow()
{
   return mainWindow;
}

void DetectorControlForm::setKeithleyName(QString keithleyName)
{
   this->keithleyName = keithleyName;
}

void DetectorControlForm::connectSignals()
{
   connect(ui->biasVoltageButton, SIGNAL(clicked(bool)), this, SLOT(biasVoltageClicked(bool)));
   connect(ui->collectImages, SIGNAL(pressed()), this, SLOT(handleCollectImagesPressed()));
   connect(ui->initialiseConnection, SIGNAL(pressed()), this, SLOT(initialiseDetectorPressed()));
   connect(ui->terminateConnection, SIGNAL(pressed()), this, SLOT(terminateDetectorPressed()));
   connect(ui->reconnect, SIGNAL(pressed()), this, SLOT(reconnectPressed()));
   connect(ui->abortDAQ, SIGNAL(pressed()), this, SLOT(abortDAQ()));
   connect(ui->imageCount, SIGNAL(valueChanged(int)), this, SLOT(handleFixedImageCountChanged(int)));
   connect(ui->setFingerTemperatureButton, SIGNAL(pressed()), this, SLOT(handleSetFingerTemperature()));
}

void DetectorControlForm::handleCollectImagesPressed()
{
   emit collectImagesPressed();
   waitingForModeChange = true;
   qDebug() <<"ui->xResolution->value(), ui->yResolution->value()" << ui->xResolution->value() << ui->yResolution->value();
   emit executeCommand(GigEDetector::CONFIGURE, ui->xResolution->value(), ui->yResolution->value());
//   emit executeCommand(GigEDetector::COLLECT, ui->imageCount->value(), 1);
}

void DetectorControlForm::abortDAQ()
{
   qDebug() <<"DetectorControlForm::abortDAQ() sending signal";
   emit abortDAQPressed();
   //   emit executeCommand(GigEDetector::ABORT, 0, 0);
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

void DetectorControlForm::biasVoltageClicked(bool biasVoltageOn)
{
    qDebug() <<"DetectorControlForm::biasVoltageClicked with " << biasVoltageOn;
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
   emit executeCommand(GigEDetector::INITIALISE, 0,0);

   /* Need to properly get status back from detector class when commnad
    * executed via signal/slot. Set GUI correctly. */
   ui->initialiseConnection->setEnabled(false);
   ui->terminateConnection->setEnabled(true);
   qDebug() << "initialiseDetectorPressed() DONE.";
}

void DetectorControlForm::terminateDetectorPressed()
{
   emit executeCommand(GigEDetector::CLOSE, 0,0);

   /* Need to properly get status back from detector class when commnad
    * executed via signal/slot. Set GUI correctly. */
   ui->initialiseConnection->setEnabled(true);
   ui->terminateConnection->setEnabled(false);
}

void DetectorControlForm::reconnectPressed()
{
   emit executeCommand(GigEDetector::RECONFIGURE, ui->xResolution->value(), ui->yResolution->value());
}

void DetectorControlForm::handleMonitorData(MonitorData *md)
{
   ui->housingTemperature->setText(QString::number(md->getTH(), 'f', 1));
   ui->housingHumidity->setText(QString::number(md->getRH(), 'f', 1));
   ui->fingerTemperature->setText(QString::number(md->getT(), 'f', 1));
   ui->dewPoint->setText(QString::number(md->getTDP(), 'f', 1));
   ui->biasCurrent->setText(QString::number(md->getIK(), 'g', 3));
   ui->detectorTemperature->setText(QString::number(md->getTASIC(), 'f', 1));
   if (firstMonitor)
   {
       ui->setFingerTemperature->setValue(md->getT());
       firstMonitor = false;
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
   if (!keithleyReservedByScripting)
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
      case DataAcquisitionStatus::WAITING_TRIGGER:
         guiWaitingTrigger();
         break;
      case DataAcquisitionStatus::TRIGGERING_STOPPED:
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
   }
}

void DetectorControlForm::handleBiasVoltageChanged(bool biasOn)
{
   ui->biasVoltageButton->setChecked(biasOn);
}

void DetectorControlForm::handleScriptReserve(QString name)
{
   if (name == keithleyName)
   {
      ui->biasVoltageButton->setEnabled(false);
      keithleyReservedByScripting = true;
   }
}

void DetectorControlForm::handleScriptRelease(QString name)
{
   if (name == keithleyName)
   {
      ui->biasVoltageButton->setEnabled(true);
      keithleyReservedByScripting = false;
   }
}

void DetectorControlForm::guiBiasRefreshing()
{
   ui->collectImages->setEnabled(false); // Disable data collection during bias refresh
   if (!keithleyReservedByScripting)
   {
      ui->biasVoltageButton->setEnabled(false); // Don't allow bias to be turned off during bias refresh
   }
}

void DetectorControlForm::handleBiasRefreshed(QString time)
{
   ui->biasLastRefreshed->setText(time);
   if (!keithleyReservedByScripting)
   {
      ui->biasVoltageButton->setEnabled(true); // Allow bias to be turned on/off again
   }
}

void DetectorControlForm::setPixmap1(QPixmap pixmap)
{
   ui->image1Label->setPixmap(pixmap);
}

void DetectorControlForm::setPixmap2(QPixmap pixmap)
{
   ui->image2Label->setPixmap(pixmap);
}

void DetectorControlForm::setPixmap3(QPixmap pixmap)
{
   ui->image3Label->setPixmap(pixmap);
}

void DetectorControlForm::setPixmap(QPixmap pixmap)
{
   ui->imageLabel->setPixmap(pixmap);
}

void DetectorControlForm::guiInitialising()
{
   ui->initialiseConnection->setEnabled(false);
   ui->terminateConnection->setEnabled(false);
   ui->reconnect->setEnabled(false);
   ui->collectImages->setEnabled(false);
   ui->abortDAQ->setEnabled(false);
}

void DetectorControlForm::guiIdle()
{
   ui->initialiseConnection->setEnabled(false);
   ui->terminateConnection->setEnabled(true);
   ui->reconnect->setEnabled(false);
   ui->abortDAQ->setEnabled(false);
}

void DetectorControlForm::guiReady()
{
   ui->initialiseConnection->setEnabled(false);
   ui->terminateConnection->setEnabled(true);
   ui->reconnect->setEnabled(true);
   ui->collectImages->setEnabled(true);
   ui->abortDAQ->setEnabled(false);
   ui->imageCount->setEnabled(true);
   ui->xResolution->setEnabled(true);
   ui->yResolution->setEnabled(true);
   if (!keithleyReservedByScripting && tAboveTdp)
   {
      ui->biasVoltageButton->setEnabled(true);
   }
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

void DetectorControlForm::guiWaitingTrigger()
{
   guiDetectorBusy();
}

void DetectorControlForm::guiDetectorBusy()
{
   ui->initialiseConnection->setEnabled(false);
   ui->terminateConnection->setEnabled(false);
   ui->reconnect->setEnabled(false);
   ui->imageCount->setEnabled(false);
   ui->collectImages->setEnabled(false);
   ui->abortDAQ->setEnabled(false);
   ui->xResolution->setEnabled(true);
   ui->yResolution->setEnabled(true);
   ui->biasVoltageButton->setEnabled(false);
}
