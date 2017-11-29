#include "processingForm.h"
#include "ui_processingForm.h"
#include "parameters.h"
#include "inifile.h"

#include <QMessageBox>
#include <QThread>
#include <QSettings>
#include <QStringList>
#include <QTime>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <Windows.h>

using namespace std;

ProcessingForm::ProcessingForm(QWidget *parent) :
   QWidget(parent),
   ui(new Ui::ProcessingForm)
{
   ui->setupUi(this);

   mainWindow = new QMainWindow();
   mainWindow->setCentralWidget(this);

   initialise();

   ui->outputDirectory->setReadOnly(true);

   connect(ui->processButton, SIGNAL(clicked()), this, SLOT(processClicked()));
   connect(ui->thresholdModeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setThresholdOptions(int)));
   connect(ui->thresholdFileButton, SIGNAL(clicked(bool)), this, SLOT(setThresholdFile()));
   connect(ui->thresholdButton, SIGNAL(clicked()), this, SLOT(setThresholdParameters()));
   connect(ui->energyCalibrationCheckBox, SIGNAL(toggled(bool)), this, SLOT(setEnergyCalibration(bool)));
   connect(ui->energyCalibrationButton, SIGNAL(clicked()), this, SLOT(setEnergyCalibrationParameters()));
   connect(ui->hxtCheckBox, SIGNAL(toggled(bool)), this, SLOT(setHxtGeneration(bool)));
   connect(ui->gradientsFileButton, SIGNAL(clicked(bool)), this, SLOT(setGradientsFile()));
   connect(ui->interceptsFileButton, SIGNAL(clicked(bool)), this, SLOT(setInterceptsFile()));
   connect(ui->hxtCheckBox, SIGNAL(toggled(bool)), this, SLOT(setHxtGeneration(bool)));
   connect(ui->binStartSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setEndSpinBoxLimit(int)));
   connect(ui->binEndSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setStartSpinBoxLimit(int)));
   connect(ui->chargedSharingComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setChargedSharingOptions(int)));
   connect(ui->chargedSharingButton, SIGNAL(clicked()), this, SLOT(setChargedSharingParameters()));
   connect(ui->inputFilesListButton, SIGNAL(clicked(bool)), this, SLOT(setInputFilesList()));
   connect(ui->outputDirectoryButton, SIGNAL(clicked(bool)), this, SLOT(setOutputDirectory()));
   connect(ui->dataFileButton, SIGNAL(clicked(bool)), this, SLOT(setDataFileParameters()));
   connect(ui->applyAllButton, SIGNAL(clicked(bool)), this, SLOT(setAllParameters()));
}

ProcessingForm::~ProcessingForm()
{
   delete ui;
}

QMainWindow *ProcessingForm::getMainWindow()
{
   return mainWindow;
}

void ProcessingForm::initialiseProcessingForm()
{
   QSettings settings(QSettings::UserScope, "TEDDI", "HexitecGigE");
   QString twoEasyFilename = Parameters::twoEasyIniFilename;
   QString inputFilesList;
   IniFile *twoEasyIniFile;
   QString filename;
   QString gradientFilename;
   QString interceptFilename;
   QString outputPrefix;
   QString boolString = "true";
//   int nRows = 80;
//   int nCols = 80;
   int rows = 80;
   int columns = 80;
   long long binStart = 0;
   long long binEnd = 100;
   double binWidth = 0.5;
   QString thresholdOptionString = "Value";
   QString chargedSharingOptionString = "None";
   QString pixelGridOptionString = "3 * 3";
   long long thresholdValue = 100;
   QString thresholdFile;
   int fileStartPos;

   filename = "DEFAULT";
   ui->thresholdModeComboBox->setCurrentIndex(1);
   ui->thresholdFile->setText("");
   ui->energyCalibrationCheckBox->setChecked(true);
   ui->totalSpectrumCheckBox->setChecked(true);
   ui->binStartSpinBox->setValue(binStart);
   ui->binEndSpinBox->setValue(binEnd);
   ui->binWidthSpinBox->setValue(binWidth);

   if (settings.contains("hexitecGigEIniFilename"))
   {
      twoEasyFilename = settings.value("hexitecGigEIniFilename").toString();
   }
   twoEasyIniFile = new IniFile(twoEasyFilename);
   QString defaultDirectory = twoEasyFilename;
   fileStartPos = defaultDirectory.lastIndexOf("/");
   defaultDirectory.truncate(fileStartPos);

   if ((boolString = twoEasyIniFile->getString("Processing/Re-order").toLower()) != QVariant(INVALID))
   {
      if (boolString == QLatin1String("true"))
      {
         ui->re_orderCheckBox->setChecked(true);
      }
      else if (boolString == QLatin1String("false"))
      {
         ui->re_orderCheckBox->setChecked(false);
      }
   }

   if ((thresholdOptionString = twoEasyIniFile->getString("Processing/Threshold_Option")) != QVariant(INVALID))
   {
      ui->thresholdModeComboBox->setCurrentText(thresholdOptionString);
   }
   setThresholdOptions(ui->thresholdModeComboBox->currentIndex());

   if ((thresholdValue = twoEasyIniFile->getInt("Processing/Threshold_Value")) != QVariant(INVALID))
   {
      ui->thresholdValue->setValue(thresholdValue);
   }

   if ((thresholdFile = twoEasyIniFile->getString("Processing/Threshold_File")) != QVariant(INVALID))
   {
      ui->thresholdFile->setText(thresholdFile);
   }
   if ((gradientFilename = twoEasyIniFile->getString("Processing/Gradient_File")) != QVariant(INVALID))
   {
      ui->gradientFilename->setText(gradientFilename);
   }
   else
   {
      ui->gradientFilename->setText(filename);
   }

   if ((interceptFilename = twoEasyIniFile->getString("Processing/Intercept_File")) != QVariant(INVALID))
   {
      ui->interceptFilename->setText(interceptFilename);
   }
   else
   {
      ui->interceptFilename->setText(filename);
   }

   boolString = "true";
   if ((boolString = twoEasyIniFile->getString("Processing/Energy_Calibration").toLower()) != QVariant(INVALID))
   {
      if (boolString == QLatin1String("true"))
      {
         ui->energyCalibrationCheckBox->setChecked(true);
      }
      else if (boolString == QLatin1String("false"))
      {
         ui->energyCalibrationCheckBox->setChecked(false);
      }
   }

   boolString = "true";
   if ((boolString = twoEasyIniFile->getString("Processing/Hxt_Generation").toLower()) != QVariant(INVALID))
   {
      if (boolString == QLatin1String("true"))
      {
         ui->hxtCheckBox->setChecked(true);
      }
      else if (boolString == QLatin1String("false"))
      {
         ui->hxtCheckBox->setChecked(false);
      }
   }
   if ((rows = twoEasyIniFile->getInt("Processing/Rows")) != QVariant(INVALID))
   {
      qDebug() << "Set rows = " << rows;
      nRows = rows;
   }
   else
   {
      qDebug() << "failed " << rows;
      nRows = 80;
   }

   if ((columns = twoEasyIniFile->getInt("Processing/Columns")) != QVariant(INVALID))
   {
      qDebug() << "Set columns = " << columns;
      nCols = columns;
   }
   else
   {
      qDebug() << "failed " << columns;
      nCols = 80;
   }
   frameSize = nRows * nCols;

   if ((binStart = twoEasyIniFile->getInt("Processing/Bin_Start")) != QVariant(INVALID))
   {
      ui->binStartSpinBox->setValue(binStart);
   }
   if ((binEnd = twoEasyIniFile->getInt("Processing/Bin_End")) != QVariant(INVALID))
   {
      ui->binEndSpinBox->setValue(binEnd);
   }
   if ((binWidth = twoEasyIniFile->getDouble("Processing/Bin_Width")) != QVariant(INVALID))
   {
      ui->binWidthSpinBox->setValue(binWidth);
   }

   ui->hxtCheckBox->setChecked(true);
   boolString = "true";
   if ((boolString = twoEasyIniFile->getString("Processing/Total_Spectrum").toLower()) != QVariant(INVALID))
   {
      if (boolString == QLatin1String("true"))
      {
         ui->totalSpectrumCheckBox->setChecked(true);
      }
      else if (boolString == QLatin1String("false"))
      {
         ui->totalSpectrumCheckBox->setChecked(false);
      }
   }

   if ((chargedSharingOptionString = twoEasyIniFile->getString("Processing/Charged_Sharing_Option")) != QVariant(INVALID))
   {
      int index = ui->chargedSharingComboBox->findText(chargedSharingOptionString);
      qDebug() << "chargedSharingOptionString = " << chargedSharingOptionString << " index " << index;
      ui->chargedSharingComboBox->setCurrentIndex(index);
   }
   if ((pixelGridOptionString = twoEasyIniFile->getString("Processing/Pixel_Grid_Option")) != QVariant(INVALID))
   {
      int index = ui->pixelGridComboBox->findText(pixelGridOptionString);
      qDebug() << "pixelGridOptionString = " << pixelGridOptionString << " index " << index;
      ui->pixelGridComboBox->setCurrentIndex(index);
   }

   if ((filename = twoEasyIniFile->getCharArray("Processing/Output_Directory")) != QVariant(INVALID))
   {
      defaultDirectory = QString(filename);
   }
   ui->outputDirectory->setText(defaultDirectory);

   filename = "DEFAULT";

   if ((outputPrefix = twoEasyIniFile->getString("Processing/Output_Prefix")) != QVariant(INVALID))
   {
      ui->outputPrefix->setText(outputPrefix);
   }
   else
   {
      ui->outputPrefix->setText(filename);
   }

   filename = "DEFAULT";

   if ((inputFilesList = twoEasyIniFile->getString("Processing/Input_File_List")) != QVariant(INVALID))
   {
      ui->inputFilesList->setText(inputFilesList);
   }
   else
   {
      ui->inputFilesList->setText(filename);
   }

   if ((nRows != 80) || (nCols !=80))
   {
      ui->re_orderCheckBox->setChecked(false);
      ui->re_orderCheckBox->setEnabled(false);
   }

   emit configureSensor(nRows, nCols);
   emit configureProcessing(ui->re_orderCheckBox->isChecked(), nextFrame,
                            ui->thresholdModeComboBox->currentIndex(), ui->thresholdValue->value(), ui->thresholdFile->text());
   emit configureProcessing(ui->energyCalibrationCheckBox->isChecked(), ui->hxtCheckBox->isChecked(),
                            ui->binStartSpinBox->value(), ui->binEndSpinBox->value(),
                            ui->binWidthSpinBox->value(), ui->totalSpectrumCheckBox->isChecked(),
                            ui->gradientFilename->text(),
                            ui->interceptFilename->text());
   emit configureProcessing(ui->chargedSharingComboBox->currentIndex(), ui->pixelGridComboBox->currentIndex());
   emit configureProcessing(ui->inputFilesList->text().split(','), ui->outputDirectory->text(), ui->outputPrefix->text());
}

void ProcessingForm::initialise()
{
   setThresholdParameters();
}

void ProcessingForm::processClicked()
{
   qDebug() << "PROCESS BUTTON has been clicked!";
//   emit processImages(nRows, nCols);
   guiBusy();
   emit processImages();
}

void ProcessingForm::setThresholdOptions(int thresholdOption)
{
   this->thresholdOption = thresholdOption;
   switch (thresholdOption)
   {
      case 0:
         ui->thresholdValue->setEnabled(false);
         ui->thresholdFile->setEnabled(false);
         ui->thresholdFileButton->setEnabled(false);
         ui->nextFrameCorrectionCheckBox->setEnabled(false);
         nextFrame = false;
         break;
      case 1:
         ui->thresholdValue->setEnabled(true);
         ui->thresholdFile->setEnabled(false);
         ui->thresholdFileButton->setEnabled(false);
         ui->nextFrameCorrectionCheckBox->setEnabled(true);
         nextFrame = ui->nextFrameCorrectionCheckBox->isChecked();
         break;
      case 2:
         ui->thresholdValue->setEnabled(false);
         ui->thresholdFile->setEnabled(true);
         ui->thresholdFileButton->setEnabled(true);
         ui->nextFrameCorrectionCheckBox->setEnabled(true);
         nextFrame = ui->nextFrameCorrectionCheckBox->isChecked();
         break;
      default:
         break;
   }
}

void ProcessingForm::setThresholdParameters()
{
   int thresholdValue;
   QString thresholdFile;

   switch (thresholdOption)
   {
      case 0:
         thresholdValue = NULL;
         nextFrame = false;
         break;
      case 1:
         thresholdValue = ui->thresholdValue->value();
         nextFrame = ui->nextFrameCorrectionCheckBox->isChecked();
         break;
      case 2:
         thresholdValue = NULL;
         thresholdFile = ui->thresholdFile->text();
         nextFrame = ui->nextFrameCorrectionCheckBox->isChecked();
         break;
      default:
         break;
   }

   emit configureProcessing(ui->re_orderCheckBox->isChecked(), nextFrame,
                            thresholdOption, thresholdValue, thresholdFile);

}

void ProcessingForm::setThresholdFile()
{
   QString source = QFileDialog::getOpenFileName(this, tr("Open Threshold File"), "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//");
   ui->thresholdFile->setText(source);
}

void ProcessingForm::setEnergyCalibration(bool energyCalibration)
{
   this->energyCalibration = energyCalibration;
   if (this->energyCalibration)
   {
      ui->gradientFilename->setEnabled(true);
      ui->gradientsFileButton->setEnabled(true);
      ui->interceptFilename->setEnabled(true);
      ui->interceptsFileButton->setEnabled(true);
      if (ui->chargedSharingComboBox->currentIndex() > 0)
      {
         ui->pixelGridComboBox->setEnabled(true);
      }
      else
      {
         ui->pixelGridComboBox->setEnabled(false);
      }
      ui->chargedSharingButton->setEnabled(true);
   }
   else
   {
      ui->gradientFilename->setEnabled(false);
      ui->gradientsFileButton->setEnabled(false);
      ui->interceptFilename->setEnabled(false);
      ui->interceptsFileButton->setEnabled(false);
   }
}

void ProcessingForm::setHxtGeneration(bool hxtGeneration)
{
   this->hxtGeneration = hxtGeneration;
   if (this->hxtGeneration)
   {
      ui->binStartSpinBox->setEnabled(true);
      ui->binEndSpinBox->setEnabled(true);
      ui->binWidthSpinBox->setEnabled(true);
   }
   else
   {
      ui->binStartSpinBox->setEnabled(false);
      ui->binEndSpinBox->setEnabled(false);
      ui->binWidthSpinBox->setEnabled(false);
   }
}

void ProcessingForm::setEnergyCalibrationParameters()
{
   int binStart;
   int binEnd;
   double binWidth;
   bool totalSpectrum;

   binStart = ui->binStartSpinBox->value();
   binEnd = ui->binEndSpinBox->value();
   binWidth = ui->binWidthSpinBox->value();
   totalSpectrum = ui->totalSpectrumCheckBox->isChecked();

   gradientFilename = ui->gradientFilename->text();
   interceptFilename = ui->interceptFilename->text();

   emit configureProcessing(energyCalibration, hxtGeneration,
                            binStart, binEnd, binWidth, totalSpectrum,
                            ui->gradientFilename->text(),
                            ui->interceptFilename->text());
}

void ProcessingForm::setGradientsFile()
{
   QString source = QFileDialog::getOpenFileName(this, tr("Open Gradients File"), "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//");
   ui->gradientFilename->setText(source);
}

void ProcessingForm::setInterceptsFile()
{
   QString source = QFileDialog::getOpenFileName(this, tr("Open Intercepts File"), "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//");
   ui->interceptFilename->setText(source);
}

void ProcessingForm::setStartSpinBoxLimit(int upperLimit)
{
   ui->binStartSpinBox->setMaximum(upperLimit - 1);
}

void ProcessingForm::setEndSpinBoxLimit(int lowerLimit)
{
   ui->binEndSpinBox->setMinimum(lowerLimit + 1);
}

void ProcessingForm::guiBusy()
{
   ui->thresholdButton->setEnabled(false);
   ui->energyCalibrationButton->setEnabled(false);
   ui->chargedSharingButton->setEnabled(false);
   ui->dataFileButton->setEnabled(false);
   ui->applyAllButton->setEnabled(false);
   ui->processButton->setEnabled(false);
}

void ProcessingForm::guiIdle()
{
   ui->thresholdButton->setEnabled(true);
   ui->energyCalibrationButton->setEnabled(true);
   ui->chargedSharingButton->setEnabled(true);
   ui->dataFileButton->setEnabled(true);
   ui->applyAllButton->setEnabled(true);
   ui->processButton->setEnabled(true);
}



void ProcessingForm::NextFrameCorrectionOption(bool nextFrameCorrection)
{
}

void ProcessingForm::setChargedSharingOptions(int chargedSharingOption)
{
   this->chargedSharingOption = chargedSharingOption;
   switch (chargedSharingOption)
   {
      case 0:
         ui->chargedSharingComboBox->setEnabled(true);
         ui->pixelGridComboBox->setEnabled(false);
         break;
      case 1:
         ui->chargedSharingComboBox->setEnabled(true);
         ui->pixelGridComboBox->setEnabled(true);
         break;
      case 2:
         ui->chargedSharingComboBox->setEnabled(true);
         ui->pixelGridComboBox->setEnabled(true);
         break;
      default:
         break;
   }
}

void ProcessingForm::setChargedSharingParameters()
{
   int pixelGridOption;

   pixelGridOption = ui->pixelGridComboBox->currentIndex();

   emit configureProcessing(chargedSharingOption, pixelGridOption);

}

void ProcessingForm::setInputFilesList()
{
   QStringList source = QFileDialog::getOpenFileNames(this, tr("Open Input Files"), "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//", "Raw Data (*.bin)");
   ui->inputFilesList->setText(source.join(", "));
}

void ProcessingForm::setOutputDirectory()
{
   QString source = QFileDialog::getExistingDirectory(this, tr("Open Output Directory"), "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//", QFileDialog::ShowDirsOnly);
   ui->outputDirectory->setText(source);
}


void ProcessingForm::setDataFileParameters()
{

   inputFilesList = ui->inputFilesList->text().split(", ");
   outputDirectory = ui->outputDirectory->text();
   outputPrefix = ui->outputPrefix->text();

   emit configureProcessing(inputFilesList, outputDirectory, outputPrefix);
}

void ProcessingForm::setAllParameters()
{
   setThresholdParameters();
   setEnergyCalibrationParameters();
   setChargedSharingParameters();
   setDataFileParameters();
}

void ProcessingForm::handleInvalidParameterFiles(bool thresholdsStatus, bool gradientsStatus, bool interceptsStatus)
{
   QMessageBox msgBox;
   QString thresholdsText = thresholdsStatus ? "valid." : "invalid - missing values set to default.";
   QString gradientsText = gradientsStatus ? "valid." : "invalid - missing values set to default.";
   QString interceptsText = interceptsStatus ? "valid." : "invalid - missing values set to default.";
   QString messageText = "\tThresholds File : " + thresholdsText
                         + "\n\tGradients File: " + gradientsText
                         + "\n\tIntercepts File: " + interceptsText;

   msgBox.setWindowTitle("Parameter File Warning");
   msgBox.setText("WARNING: Some of the processing parameter files are invalid.");
   msgBox.setInformativeText(messageText);
   msgBox.setStandardButtons(QMessageBox::Ok);
   msgBox.setDefaultButton(QMessageBox::Ok);
   msgBox.exec();
}

void ProcessingForm::handleProcessingComplete()
{
   guiIdle();
}

void ProcessingForm::handleImageStarted()
{
   guiBusy();
}

void ProcessingForm::handleDetectorResolutionSet(unsigned char xRes, unsigned char yRes)
{
   qDebug() << "ProcessingForm::handleDetectorResolutionSet";
   if ((xRes != 80) || (yRes !=80))
   {
      ui->re_orderCheckBox->setChecked(false);
      ui->re_orderCheckBox->setEnabled(false);
   }

   emit configureProcessing(ui->re_orderCheckBox->isChecked(), nextFrame,
                            ui->thresholdModeComboBox->currentIndex(), ui->thresholdValue->value(), ui->thresholdFile->text());

}
