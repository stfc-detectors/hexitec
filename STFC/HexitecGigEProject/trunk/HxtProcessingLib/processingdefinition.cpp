#include "processingdefinition.h"
#include <QSettings>
#include <QDebug>
#include <iostream>
#include <fstream>

using namespace std;

ProcessingDefinition::ProcessingDefinition(long long frameSize)
{
   gradientFilename =  new char[1024];
   interceptFilename =  new char[1024];
   this->frameSize = frameSize;
   gradientValue = (double *) calloc(frameSize, sizeof(double));
   interceptValue = (double *) calloc(frameSize, sizeof(double));
   thresholdPerPixel = (uint16_t *) calloc(frameSize, sizeof(uint16_t));
   outputDirectory = new char[1024];
   outputPrefix = new char[1024];
}

ProcessingDefinition::~ProcessingDefinition()
{
   delete outputDirectory;
   delete outputPrefix;
   delete gradientFilename;
   delete interceptFilename;
   delete gradientValue;
   delete interceptValue;
   delete thresholdPerPixel;
}

void ProcessingDefinition::setThresholdMode(ThresholdMode threshholdMode)
{
   this->threshholdMode = threshholdMode;
}

void ProcessingDefinition::setThresholdValue(int thresholdValue)
{
   this->thresholdValue = thresholdValue;
}

void ProcessingDefinition::setThresholdPerPixel(char * thresholdFilename)
{
   getData(thresholdFilename, thresholdPerPixel);
}

void ProcessingDefinition::setEnergyCalibration(bool energyCalibration)
{
   this->energyCalibration = energyCalibration;
}


void ProcessingDefinition::setGradientFilename(char *gradientFilename)
{
   strcpy(this->gradientFilename, (const char *)gradientFilename);
   setGradients();
}

void ProcessingDefinition::setInterceptFilename(char *interceptFilename)
{
   strcpy(this->interceptFilename, (const char *)interceptFilename);
   setIntercepts();
}

ThresholdMode ProcessingDefinition::getThreshholdMode() const
{
   return threshholdMode;
}

int ProcessingDefinition::getThresholdValue() const
{
   return thresholdValue;
}

uint16_t *ProcessingDefinition::getThresholdPerPixel() const
{
   return thresholdPerPixel;
}

char *ProcessingDefinition::getGradientFilename()
{
   return gradientFilename;
}

char *ProcessingDefinition::getInterceptFilename()
{
   return interceptFilename;
}

char *ProcessingDefinition::getProcessedFilename() const
{
   return processedFilename;
}

double *ProcessingDefinition::getGradients()
{
//   setGradients();
   return gradientValue;
}

double *ProcessingDefinition::getIntercepts()
{
   return interceptValue;
}

void ProcessingDefinition::setGradients()
{
   getData(gradientFilename, gradientValue);
}

void ProcessingDefinition::setIntercepts()
{
   getData(interceptFilename, interceptValue);
}

void ProcessingDefinition::getData(char *filename, double *dataValue)
{
   int i = 0;
   std::ifstream inFile;

   inFile.open(filename);

   if (!inFile)
     qDebug() << "ProcessingDefinition::getData - error opening " << filename;
   while (inFile >> dataValue[i])
   {
      i++;
   }

   if (i < 6400)
     qDebug() << "error: only " << i << " could be read";
   else
     qDebug() << "file read OK ";
   inFile.close();
}

void ProcessingDefinition::getData(const char *filename, uint16_t *dataValue)
{
   int i = 0;
   std::ifstream inFile;

   inFile.open(filename);

   if (!inFile)
     qDebug() << "ProcessingDefinition::getData - error opening " << filename;
   while (inFile >> dataValue[i])
   {
      i++;
   }

   if (i < 6400)
     qDebug() << "error: only " << i << " could be read";
   else
     qDebug() << "file read OK ";
   inFile.close();
}

int ProcessingDefinition::getPixelGridSize() const
{
    return pixelGridSize;
}

void ProcessingDefinition::setPixelGridSize(int value)
{
   switch (value)
   {
      case 0:
         pixelGridSize = 3;
         break;
      case 1:
         pixelGridSize = 5;
         break;
      default:
         break;
   }
   qDebug() << "value: " << value << "ProcessingDefinition::setPixelGridSize(): " << pixelGridSize;
}

bool ProcessingDefinition::getNextFrameCorrection() const
{
    return nextFrameCorrection;
}

void ProcessingDefinition::setNextFrameCorrection(bool value)
{
    nextFrameCorrection = value;
}

ChargedSharingMode ProcessingDefinition::getChargedSharingMode() const
{
    return chargedSharingMode;
}

void ProcessingDefinition::setChargedSharingMode(const ChargedSharingMode &value)
{
    chargedSharingMode = value;
}

long long ProcessingDefinition::getHxtBufferHeaderSize() const
{
    return hxtBufferHeaderSize;
}

void ProcessingDefinition::setHxtBufferHeaderSize(long long value)
{
    hxtBufferHeaderSize = value;
}

long long ProcessingDefinition::getHxtBufferAllDataSize() const
{
   return hxtBufferAllDataSize;
}

long long ProcessingDefinition::getFrameSize() const
{
   return frameSize;
}

void ProcessingDefinition::setFrameSize(long long value)
{
   frameSize = value;
}

char *ProcessingDefinition::getOutputPrefix()
{
   return outputPrefix;
}

void ProcessingDefinition::setOutputPrefix(char *value)
{
   strcpy(this->outputPrefix, value);
}

char *ProcessingDefinition::getOutputDirectory()
{
   return outputDirectory;
}

void ProcessingDefinition::setOutputDirectory(char  *value)
{
   strcpy(this->outputDirectory, value);
}

bool ProcessingDefinition::getTotalSpectrum() const
{
   return totalSpectrum;
}

void ProcessingDefinition::setTotalSpectrum(bool totalSpectrum)
{
   this->totalSpectrum = totalSpectrum;
   nBins = (int)(((binEnd - binStart) / binWidth) + 0.5);
   hxtBufferAllDataSize = ((nBins * frameSize) + nBins) * sizeof(double);
   hxtBufferHeaderSize = sizeof(HxtItem::HxtV3Buffer) - sizeof(double *);
}

double ProcessingDefinition::getBinWidth() const
{
    return binWidth;
}

void ProcessingDefinition::setBinWidth(const double &binWidth)
{
   this->binWidth = binWidth;
}

long long ProcessingDefinition::getNBins()
{
   return nBins;
}

long long ProcessingDefinition::getBinEnd() const
{
   return binEnd;
}

void ProcessingDefinition::setBinEnd(const long long &binEnd)
{
   this->binEnd = binEnd;
}

long long ProcessingDefinition::getBinStart() const
{
   return binStart;
}

void ProcessingDefinition::setBinStart(const long long &binStart)
{
   this->binStart = binStart;
}

bool ProcessingDefinition::getEnergyCalibration() const
{
   return energyCalibration;
}

bool ProcessingDefinition::getRe_order() const
{
   return re_order;
}

void ProcessingDefinition::setRe_order(bool re_order)
{
    this->re_order = re_order;
}

