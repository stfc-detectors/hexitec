#include "processingdefinition.h"
#include <iostream>
#include <fstream>
///
#include <QDebug>
using namespace std;

ProcessingDefinition::ProcessingDefinition(long long frameInSize)
{
   gradientFilename =  new char[1024];
   interceptFilename =  new char[1024];
   this->frameInSize = frameInSize;
   gradientValue = static_cast<double *>(calloc(frameInSize, sizeof(double)));
   interceptValue = static_cast<double *>(calloc(frameInSize, sizeof(double)));
   thresholdPerPixel = static_cast<uint16_t *>(calloc(frameInSize, sizeof(uint16_t)));
   outputDirectory = new char[1024];
   outputPrefix = new char[1024];
   nInRows = 20;
   nInCols = 80;
   nOutRows = 4;
   nOutCols = 16;
}

ProcessingDefinition::~ProcessingDefinition()
{
   delete outputDirectory;
   delete outputPrefix;
   delete gradientFilename;
   delete interceptFilename;
   free(gradientValue);
   free(interceptValue);
   free(thresholdPerPixel);
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
   strcpy(this->gradientFilename, const_cast<char *>(gradientFilename));
   setGradients();

   return gradientsStatus;
}

bool ProcessingDefinition::setInterceptFilename(char *interceptFilename)
{
   strcpy(this->interceptFilename, const_cast<char *>(interceptFilename));
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
     for (int val = 0; val < frameInSize; val ++)
     {
        dataValue[val] = defaultValue;
     }
   }

   while ((i < frameInSize) && (inFile >> dataValue[i]))
   {
      i++;
   }

   if (i < frameInSize)
   {
      for (int val = i; val < frameInSize; val ++)
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
   bool success = false;

   inFile.open(filename);

   if (!inFile)
   {
     for (int val = 0; val < frameInSize; val ++)
     {
        dataValue[val] = defaultValue;
     }
   }

   while ((i < frameInSize) && (inFile >> dataValue[i]))
   {
     i++;
   }

   if (i < frameInSize)
   {
      for (int val = i; val < frameInSize; val ++)
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

uint32_t ProcessingDefinition::getFrameInRows()
{
   return nInRows;
}

void ProcessingDefinition::setFrameInRows(uint32_t nInRows)
{
   this->nInRows = nInRows;
}

uint32_t ProcessingDefinition::getFrameInCols()
{
   return nInCols;
}

void ProcessingDefinition::setFrameInCols(uint32_t nInCols)
{
   this->nInCols = nInCols;
}

uint32_t ProcessingDefinition::getFrameOutRows()
{
   return nOutRows;
}

void ProcessingDefinition::setFrameOutRows(uint32_t nOutRows)
{
   this->nOutRows = nOutRows;
}

uint32_t ProcessingDefinition::getFrameOutCols()
{
   return nOutCols;
}

void ProcessingDefinition::setFrameOutCols(uint32_t nOutCols)
{
   this->nOutCols = nOutCols;
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

long long ProcessingDefinition::getHxtBufferAllDataSize() const
{
   return hxtBufferAllDataSize;
}

uint32_t ProcessingDefinition::getFrameInSize() const
{
   return frameInSize;
}

void ProcessingDefinition::setFrameInSize(uint32_t value)
{
   frameInSize = value;
}

uint32_t ProcessingDefinition::getFrameOutSize() const
{
   return frameOutSize;
}

void ProcessingDefinition::setFrameOutSize(uint32_t value)
{
   frameOutSize = value;
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
   nBins = static_cast<int>(((binEnd - binStart) / binWidth) + 0.5);
   hxtBufferAllDataSize = ((nBins * frameOutSize) + nBins) * static_cast<long long>(sizeof(double));
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

