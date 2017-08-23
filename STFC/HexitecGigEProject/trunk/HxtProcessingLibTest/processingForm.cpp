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
   connect(ui->re_orderCheckBox, SIGNAL(toggled(bool)), this, SLOT(setRe_orderOption(bool)));
   connect(ui->thresholdModeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setThresholdOptions(int)));
   connect(ui->thresholdFileButton, SIGNAL(clicked(bool)), this, SLOT(setThresholdFile()));
   connect(ui->thresholdButton, SIGNAL(clicked()), this, SLOT(setThresholdParameters()));
   connect(ui->energyCalibrationCheckBox, SIGNAL(toggled(bool)), this, SLOT(setEnergyCalibration(bool)));
   connect(ui->energyCalibrationButton, SIGNAL(clicked()), this, SLOT(setEnergyCalibrationParameters()));
   connect(ui->hxtCheckBox, SIGNAL(toggled(bool)), this, SLOT(setHxtGeneration(bool)));
   connect(ui->binStartSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setEndSpinBoxLimit(int)));
   connect(ui->binEndSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setStartSpinBoxLimit(int)));
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
   setThresholdParameters();
}

/*
void ProcessingForm::processClicked()
{
   rotate90();
   rotate180();
   rotate270();
}
*/

void ProcessingForm::processClicked()
{
   const char *imageFilename = "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//image.bin";

   qDebug() << "PROCESS BUTTON has been clicked!";
   processImage(imageFilename, "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//Size10.bin");
}

void ProcessingForm::setRe_orderOption(bool re_order)
{
   char *gradientFilename;
   char *interceptFilename;
   char *processedFilename;

   emit configureProcessing(re_order,
                            gradientFilename,
                            interceptFilename,
                            processedFilename);
}

void ProcessingForm::processImage(const char *imageFilename, const char *filename)
{
   char *transferBuffer;
   unsigned long validFrames = 0;
   long long totalFramesAcquired = 0;
   std::ifstream inFile;
   int temp = 0;
   int nRows = 80;
   int nCols = 80;

   inFile.open(filename, ifstream::binary);
   emit imageStarted(imageFilename, nRows, nCols);

   int bufferCount = 0;

   while (inFile)
   {
      transferBuffer = (char *) calloc(6400 * 500 * sizeof(uint16_t), sizeof(char));

      inFile.read(transferBuffer, 6400 * 500 * 2);
      if (!inFile)
      {
         validFrames = inFile.gcount() / (6400 * 2);
         qDebug() << validFrames <<" valid frames could be read: " << temp++;
         emit transferBufferReady(transferBuffer, validFrames);
      }
      else
      {
         validFrames = inFile.gcount() / (6400 * 2);
         qDebug() << validFrames <<" valid frames could be read: " << temp++;
         emit transferBufferReady(transferBuffer, 500);
      }
      totalFramesAcquired += validFrames;
      bufferCount++;
      Sleep(50);
   }
   inFile.close();
   emit imageComplete(totalFramesAcquired);
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
         break;
      case 1:
         ui->thresholdValue->setEnabled(true);
         ui->thresholdFile->setEnabled(false);
         ui->thresholdFileButton->setEnabled(false);
         break;
      case 2:
         ui->thresholdValue->setEnabled(false);
         ui->thresholdFile->setEnabled(true);
         ui->thresholdFileButton->setEnabled(true);
         break;
      default:
         break;
   }
}

void ProcessingForm::setThresholdParameters()
{
   int thresholdValue;
   uint16_t *thresholdPerPixel;
   char *gradientFilename;
   char *interceptFilename;
   char *processedFilename;
   char *thresholdFile;

   switch (thresholdOption)
   {
      case 0:
         thresholdValue = NULL;
         thresholdPerPixel = NULL;
         break;
      case 1:
         thresholdValue = ui->thresholdValue->value();
         thresholdPerPixel = NULL;
         break;
      case 2:
         thresholdValue = NULL;
         thresholdFile = (char *)ui->thresholdFile->text().toStdString().c_str();
         readThresholdFile(thresholdFile);
         break;
      default:
         break;
   }
   gradientFilename = (char *)"C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//m_gradients.txt";
   interceptFilename = (char *)"C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//c_intercepts.txt";
   emit configureProcessing(thresholdOption, thresholdValue, thresholdPerPixel,
                            gradientFilename,
                            interceptFilename,
                            processedFilename);
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
      ui->gradientsFile->setEnabled(true);
      ui->gradientsFileButton->setEnabled(true);
      ui->interceptsFile->setEnabled(true);
      ui->interceptsFileButton->setEnabled(true);
   }
   else
   {
      ui->gradientsFile->setEnabled(false);
      ui->gradientsFileButton->setEnabled(false);
      ui->interceptsFile->setEnabled(false);
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
   char *gradientFilename;
   char *interceptFilename;
   char *processedFilename;

   binStart = ui->binStartSpinBox->value();
   binEnd = ui->binEndSpinBox->value();
   binWidth = ui->binWidthSpinBox->value();
   totalSpectrum = ui->totalSpectrumCheckBox->isChecked();

   gradientFilename = (char *)"C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//m_gradients.txt";
   interceptFilename = (char *)"C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//c_intercepts.txt";
   emit configureProcessing(energyCalibration, binStart, binEnd, binWidth, totalSpectrum,
                            gradientFilename,
                            interceptFilename,
                            processedFilename);
}

void ProcessingForm::setStartSpinBoxLimit(int upperLimit)
{
   ui->binStartSpinBox->setMaximum(upperLimit);
}

void ProcessingForm::setEndSpinBoxLimit(int lowerLimit)
{
   ui->binEndSpinBox->setMinimum(lowerLimit);
}

void ProcessingForm::readThresholdFile(char *thresholdFile)
{
   int i = 0;
   std::ifstream inFile;

   inFile.open(thresholdFile);

   if (!inFile)
     qDebug() << "error opening " << thresholdFile;
   while (inFile >> thresholdPerPixel[i])
   {
      qDebug() << "thresholdPerPixel[i]: " << thresholdPerPixel[i];
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
   char *gradientFilename;
   char *interceptFilename;
   char *processedFilename;

   emit configureProcessing(nextFrameCorrection,
                            gradientFilename,
                            interceptFilename,
                            processedFilename);
}

void ProcessingForm::setChargedSharingOptions(int chargedSharingOption)
{
   this->chargedSharingOption = chargedSharingOption;
   switch (chargedSharingOption)
   {
      case 0:
         ui->chargedSharingComboBox->setEnabled(false);
         ui->thresholdFileButton->setEnabled(false);
         break;
      case 1:
         ui->chargedSharingComboBox->setEnabled(true);
         ui->thresholdFileButton->setEnabled(false);
         break;
      case 2:
         ui->chargedSharingComboBox->setEnabled(false);
         ui->thresholdFileButton->setEnabled(true);
         break;
      default:
         break;
   }
}

void ProcessingForm::setChargedSharingParameters()
{
   int chargedSharingValue;
   char *gradientFilename;
   char *interceptFilename;
   char *processedFilename;

   switch (chargedSharingOption)
   {
      case 0:
         chargedSharingValue = NULL;
         break;
      case 1:
      case 2:
 //        chargedSharingValue = ui->chargedSharingComboBox->
         break;
      default:
         break;
   }

   gradientFilename = (char *)"C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//m_gradients.txt";
   interceptFilename = (char *)"C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//c_intercepts.txt";
/*
   emit configureProcessing(chargedSharingOption, chargedSharingValue,
                            gradientFilename,
                            interceptFilename,
                            processedFilename);
                            */
}
