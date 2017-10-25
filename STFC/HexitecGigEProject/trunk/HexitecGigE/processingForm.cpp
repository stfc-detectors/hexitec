#include "processingForm.h"
#include "ui_processingForm.h"
#include "parameters.h"
#include "inifile.h"

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
//   initialiseProcessingForm();
//   connect(HxtProcessor,                         SIGNAL(hexitechBufferToDisplay(unsigned short*, QString)),
//                 reinterpret_cast<const QObject*>(mw), SLOT(readBuffer(unsigned short*, QString)));
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
   int rows;
   int columns;
   long long frameSize;
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


   if ((rows = twoEasyIniFile->getInt("Processing/Rows")) != QVariant(INVALID))
   {
      qDebug() << "Set rows = " << rows;
   }
   else
   {
      qDebug() << "failed " << rows;
      rows = 80;
   }
   if ((columns = twoEasyIniFile->getInt("Processing/Columns")) != QVariant(INVALID))
   {
      qDebug() << "Set columns = " << columns;
   }
   else
   {
      qDebug() << "failed " << columns;
      columns = 80;
   }
   frameSize = rows * columns;

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
      ui->chargedSharingComboBox->setCurrentText(chargedSharingOptionString);
   }
   if ((pixelGridOptionString = twoEasyIniFile->getString("Processing/Pixel_grid_Option")) != QVariant(INVALID))
   {
      ui->pixelGridComboBox->setCurrentText(pixelGridOptionString);
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

   emit configureSensor(rows, columns, frameSize);
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
//   gradientFilename =  new char[1024];
//   interceptFilename =  new char[1024];
//   outputDirectory =  new char[1024];
//   outputPrefix =  new char[1024];
//   initialiseProcessingForm();

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

   emit configureProcessing(inputFilesList, outputDirectory, outputPrefix);
}
