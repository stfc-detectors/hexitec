#include "processingdefinition.h"
#include <QSettings>
#include <QDebug>
#include <iostream>
#include <fstream>

ProcessingDefinition::ProcessingDefinition(long long frameSize)
{
   this->frameSize = frameSize;
   gradientValue = (double *) calloc(frameSize, sizeof(double));
   interceptValue = (double *) calloc(frameSize, sizeof(double));
   outputDirectory = "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images";
   outputPrefix = "re_order";
}

void ProcessingDefinition::setThresholdMode(ThresholdMode threshholdMode)
{
   this->threshholdMode = threshholdMode;
}

void ProcessingDefinition::setThresholdValue(int thresholdValue)
{
   this->thresholdValue = thresholdValue;
}

void ProcessingDefinition::setThresholdPerPixel(uint16_t *thresholdPerPixel)
{
   this->thresholdPerPixel = thresholdPerPixel;
}

void ProcessingDefinition::setEnergyCalibration(bool energyCalibration)
{
   this->energyCalibration = energyCalibration;
   qDebug() << "ProcessingDefinition::setEnergyCalibration: " << energyCalibration;
}

void ProcessingDefinition::setProcessedFilename(const char *processedFilename)
{
   this->processedFilename = (char *)processedFilename;
}

void ProcessingDefinition::setGradientFilename(const char *gradientFilename)
{
   this->gradientFilename = (char *)gradientFilename;
}

void ProcessingDefinition::setInterceptFilename(const char *interceptFilename)
{
   this->interceptFilename = (char *)interceptFilename;
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

char *ProcessingDefinition::getGradientFilename() const
{
   return gradientFilename;
}

char *ProcessingDefinition::getInterceptFilename() const
{
   return interceptFilename;
}

char *ProcessingDefinition::getProcessedFilename() const
{
   return processedFilename;
}

double *ProcessingDefinition::getGradients()
{
   setGradients();
   return gradientValue;
}

double *ProcessingDefinition::getIntercepts()
{
   setIntercepts();
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

void ProcessingDefinition::getData(const char *filename, double *dataValue)
{
   int i = 0;
   std::ifstream inFile;

   inFile.open(filename);

   if (!inFile)
     qDebug() << "error opening " << filename;
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
    pixelGridSize = value;
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

string ProcessingDefinition::getOutputPrefix() const
{
   return outputPrefix;
}

void ProcessingDefinition::setOutputPrefix(const string &value)
{
   outputPrefix = value;
}

string ProcessingDefinition::getOutputDirectory() const
{
   return outputDirectory;
}

void ProcessingDefinition::setOutputDirectory(const string &value)
{
   outputDirectory = value;
}

bool ProcessingDefinition::getTotalSpectrum() const
{
   return totalSpectrum;
}

void ProcessingDefinition::setTotalSpectrum(bool totalSpectrum)
{
   long long nBins;

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
    qDebug() << "Set binWidth = " << binWidth;
    this->binWidth = binWidth;
}

long long ProcessingDefinition::getBinEnd() const
{
   return binEnd;
}

void ProcessingDefinition::setBinEnd(const long long &binEnd)
{
   qDebug() << "Set binEnd = " << binEnd;
   this->binEnd = binEnd;
}

long long ProcessingDefinition::getBinStart() const
{
   return binStart;
}

void ProcessingDefinition::setBinStart(const long long &binStart)
{
   qDebug() << "Set binStart = " << binStart;
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

