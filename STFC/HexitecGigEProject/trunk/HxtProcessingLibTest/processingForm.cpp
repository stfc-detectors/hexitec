#include "processingForm.h"
#include "ui_processingForm.h"
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
}

ProcessingForm::~ProcessingForm()
{
   delete ui;
}

QMainWindow *ProcessingForm::getMainWindow()
{
   return mainWindow;
}

void ProcessingForm::initialise()
{
   qDebug()<< "ProcessingForm::initialise()";
   gradientFilename =  new char[1024];
   interceptFilename =  new char[1024];
//   outputDirectory =  new char[1024];
//   outputPrefix =  new char[1024];

   setThresholdParameters();
}

void ProcessingForm::processClicked()
{
   qDebug() << "PROCESS BUTTON has been clicked!";
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
   char *thresholdFile;

   switch (thresholdOption)
   {
      case 0:
         thresholdValue = NULL;
         thresholdFile = NULL;
         nextFrame = false;
         break;
      case 1:
         thresholdValue = ui->thresholdValue->value();
         thresholdFile = NULL;
         nextFrame = ui->nextFrameCorrectionCheckBox->isChecked();
         break;
      case 2:
         thresholdValue = NULL;
         thresholdFile = (char *)ui->thresholdFile->text().toStdString().c_str();
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
      ui->chargedSharingComboBox->setEnabled(true);
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
      ui->chargedSharingComboBox->setEnabled(false);
      ui->pixelGridComboBox->setEnabled(false);
      ui->chargedSharingButton->setEnabled(false);
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

//   gradientFilename = (char *)ui->gradientFilename->text().toStdString().c_str();
//   interceptFilename = (char *)ui->interceptFilename->text().toStdString().c_str();
   strcpy(gradientFilename, (char *)ui->gradientFilename->text().toStdString().c_str());
   strcpy(interceptFilename, (char *)ui->interceptFilename->text().toStdString().c_str());

   emit configureProcessing(energyCalibration, binStart, binEnd, binWidth, totalSpectrum,
                            gradientFilename,
                            interceptFilename,
                            processedFilename);
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

void ProcessingForm::readThresholdFile(char *thresholdFile)
{
   int i = 0;
   std::ifstream inFile;

//   thresholdFile = (char *)"C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//Thresholds_500V_28C_135_1_5clk_1501190172.txt";
   inFile.open(thresholdFile);

   if (!inFile)
   {
     qDebug() << "ProcessingForm::readThresholdFile - error opening " << thresholdFile;
   }

   while (inFile >> thresholdPerPixel[i])
   {
      i++;
   }

   if (i < 6400)
     qDebug() << "error: only " << i << " could be read";
   else
     qDebug() << "threshold file read OK ";
   inFile.close();

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

   qDebug() << "setDataFileParameters() outputPrefix = " << outputPrefix;
   emit configureProcessing(inputFilesList, outputDirectory, outputPrefix);
}
