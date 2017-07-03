#include "processingForm.h"
#include "ui_processingForm.h"
#include <QDebug>
#include <iostream>
#include <fstream>

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
   const char *imageFilename = "C://karen//STFC//Technical//PLTest//image.bin";

   qDebug() << "PROCESS BUTTON has been clicked!";
   processImage(imageFilename, "C://karen//STFC//Technical//DSoFt_Images//Size10.bin");
}

void ProcessingForm::processImage(const char *imageFilename, const char *filename)
{
   char *transferBuffer;
   unsigned long validFrames = 0;
   unsigned long long totalFramesAcquired = 0;
   std::ifstream inFile;

   inFile.open(filename, ifstream::binary);
   emit imageStarted(imageFilename, 6400 * 2);

   int bufferCount = 0;

   while (inFile)
   {
      transferBuffer = (char *) calloc(6400 * 500, sizeof(uint16_t));

      inFile.read(transferBuffer, 6400 * 500 * 2);
      if (!inFile)
      {
         validFrames = inFile.gcount() / (6400 * 2);
         qDebug() << validFrames <<" valid frames could be read";
         emit transferBufferReady(transferBuffer, validFrames);
      }
      else
      {
         validFrames = inFile.gcount() / (6400 * 2);
         qDebug() << validFrames <<" valid frames could be read";
         emit transferBufferReady(transferBuffer, 500);
      }
      totalFramesAcquired += validFrames;
      bufferCount++;
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
   gradientFilename = (char *)"C://karen//STFC//Technical//PLTest//c_gradients.txt";
   interceptFilename = (char *)"C://karen//STFC//Technical//PLTest//m_intercepts.txt";
   emit configureProcessing(thresholdOption, thresholdValue, thresholdPerPixel,
                            gradientFilename,
                            interceptFilename,
                            processedFilename);
}

void ProcessingForm::setThresholdFile()
{
   QString source = QFileDialog::getOpenFileName(this, tr("Open Threshold File"), "C://karen//STFC//Technical//PLTest//");
   ui->thresholdFile->setText(source);
}

void ProcessingForm::readThresholdFile(char *thresholdFile)
{
   std::ifstream inFile;

   inFile.open(thresholdFile, ifstream::binary);
   if (inFile.good())
   {
      inFile.read((char *)&thresholdPerPix[0], 6400 * sizeof(uint16_t));
      if (!inFile)
         qDebug() << "error: only " << inFile.gcount() << " could be read";
      else
         qDebug() << "frame read OK ";
      inFile.close();
   }
   else
   {
      qDebug() << "Threshold file open failed.";
   }
}

