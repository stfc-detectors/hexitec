#include "processingdefinition.h"
#include <QSettings>
#include <QDebug>
#include <iostream>
#include <fstream>

ProcessingDefinition::ProcessingDefinition()
{
   gradientValue = (double *) calloc(6400, sizeof(double));
   interceptValue = (double *) calloc(6400, sizeof(double));
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

bool ProcessingDefinition::getTotalSpectrum() const
{
    return totalSpectrum;
}

void ProcessingDefinition::setTotalSpectrum(bool totalSpectrum)
{
    this->totalSpectrum = totalSpectrum;
}

unsigned long long ProcessingDefinition::getBinWidth() const
{
    return binWidth;
}

void ProcessingDefinition::setBinWidth(const unsigned long long &binWidth)
{
    this->binWidth = binWidth;
}

unsigned long long ProcessingDefinition::getBinEnd() const
{
   return binEnd;
}

void ProcessingDefinition::setBinEnd(const unsigned long long &binEnd)
{
   this->binEnd = binEnd;
}

unsigned long long ProcessingDefinition::getBinStart() const
{
   return binStart;
}

void ProcessingDefinition::setBinStart(const unsigned long long &binStart)
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

