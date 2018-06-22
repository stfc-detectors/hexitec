#include "processingdefinition.h"
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

bool ProcessingDefinition::setThresholdPerPixel(char * thresholdFilename)
{
   uint16_t defaultValue = 0;
   thresholdsStatus = getData(thresholdFilename, thresholdPerPixel, defaultValue);

   return thresholdsStatus;
}

void ProcessingDefinition::setEnergyCalibration(bool energyCalibration)
{
   this->energyCalibration = energyCalibration;
}

void ProcessingDefinition::setHxtGeneration(bool hxtGeneration)
{
   this->hxtGeneration = hxtGeneration;
}

bool ProcessingDefinition::setGradientFilename(char *gradientFilename)
{
   strcpy(this->gradientFilename, (const char *)gradientFilename);
   setGradients();

   return gradientsStatus;
}

bool ProcessingDefinition::setInterceptFilename(char *interceptFilename)
{
   strcpy(this->interceptFilename, (const char *)interceptFilename);
   setIntercepts();

   return interceptsStatus;
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
   double defaultValue = 1;
   gradientsStatus = getData(gradientFilename, gradientValue, defaultValue);
}

void ProcessingDefinition::setIntercepts()
{
   double defaultValue = 0;
   interceptsStatus = getData(interceptFilename, interceptValue, defaultValue);
}

bool ProcessingDefinition::getData(char *filename, double *dataValue, double defaultValue)
{
   int i = 0;
   std::ifstream inFile;
   bool success = false;

   inFile.open(filename);

   if (!inFile)
   {
     for (int val = 0; val < frameSize; val ++)
     {
        dataValue[val] = defaultValue;
     }
   }

   while (inFile >> dataValue[i])
   {
      i++;
   }

   if (i < frameSize)
   {
      for (int val = i; val < frameSize; val ++)
      {
         dataValue[val] = defaultValue;
      }
   }
   else
   {
     success = true;
   }
   inFile.close();

   return success;
}

bool ProcessingDefinition::getData(const char *filename, uint16_t *dataValue, uint16_t defaultValue)
{
   int i = 0;
   std::ifstream inFile;
   bool success;

   inFile.open(filename);

   if (!inFile)
   {
     for (int val = 0; val < frameSize; val ++)
     {
        dataValue[val] = defaultValue;
     }
   }

   while (inFile >> dataValue[i])
   {
      i++;
   }

   if (i < frameSize)
   {
      for (int val = i; val < frameSize; val ++)
      {
         dataValue[val] = defaultValue;
      }
   }
   else
   {
     success = true;
   }
   inFile.close();

   return success;
}

int ProcessingDefinition::getRows()
{
   return nRows;
}

void ProcessingDefinition::setRows(int nRows)
{
   this->nRows = nRows;
}

int ProcessingDefinition::getCols()
{
   return nCols;
}

void ProcessingDefinition::setCols(int nCols)
{
   this->nCols = nCols;
}

bool ProcessingDefinition::getThresholdsStatus()
{
   return thresholdsStatus;
}

bool ProcessingDefinition::getGradientsStatus()
{
   return gradientsStatus;
}

bool ProcessingDefinition::getInterceptsStatus()
{
   return interceptsStatus;
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

bool ProcessingDefinition::getHxtGeneration() const
{
   return hxtGeneration;
}

bool ProcessingDefinition::getRe_order() const
{
   return re_order;
}

void ProcessingDefinition::setRe_order(bool re_order)
{
    this->re_order = re_order;
}

