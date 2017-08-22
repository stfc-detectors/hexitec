#include "hxtitem.h"
#include <QMutexLocker>
#include <QDebug>
#include <QThread>
#include <iostream>
#include <math.h>

using namespace std;


HxtItem::HxtItem(int nRows, int nCols, long long binStart, long long binEnd, double binWidth)
{
   this->frameSize = nRows * nCols;
   setBinStart(binStart);
   setBinEnd(binEnd);
   setBinWidth(binWidth);
   nBins = (int)(((binEnd - binStart) / binWidth) + 0.5);
   initialiseHxtBuffer(nRows, nCols);

   pixelEnergy = NULL;
   pixelEnergyMap = NULL;
   energiesProcessed = 0;
   this->pixelEnergyQueue.clear();
   this->pixelEnergyMapQueue.clear();
}

void HxtItem::initialiseHxtBuffer(int nRows, int nCols)
{
   double currentBin;

   strncpy(hxtV3Buffer.hxtLabel, "HEXITECH", sizeof(hxtV3Buffer.hxtLabel));
   hxtV3Buffer.hxtVersion = 3;
   hxtV3Buffer.filePrefixLength = 0;
   strncpy(hxtV3Buffer.filePrefix, "(blank)", 7);
   hxtV3Buffer.nRows = nRows;
   hxtV3Buffer.nCols = nCols;
   hxtV3Buffer.nBins = nBins;
   hxtV3Buffer.allData = (double *) calloc((nBins * frameSize) + nBins, sizeof(double));
   energyBin = hxtV3Buffer.allData;
   histogramPerPixel = hxtV3Buffer.allData + nBins;

   currentBin = binStart;
   for (long long i = binStart; i < nBins; i++, currentBin += binWidth)
   {
      *energyBin = currentBin;
      energyBin++;
   }
}

void HxtItem::enqueuePixelEnergy(double *pixelEnergy)
{
   incrementTotalEnergiesToProcess();
   QMutexLocker locker(&mutex);
   pixelEnergyQueue.enqueue(pixelEnergy);
}

double *HxtItem::getNextPixelEnergy()
{
   if (pixelEnergy != NULL)
   {
      //free(pixelEnergy);
   }

   if (pixelEnergyQueueNotEmpty())
   {
      QMutexLocker locker(&mutex);
      pixelEnergy = pixelEnergyQueue.dequeue();
   }

   return pixelEnergy;
}

bool HxtItem::pixelEnergyQueueNotEmpty()
{
   QMutexLocker locker(&mutex);
   return !pixelEnergyQueue.isEmpty();
}

int HxtItem::getPixelEnergyQueueSize()
{
   QMutexLocker locker(&mutex);
   return pixelEnergyQueue.size();
}





void HxtItem::enqueuePixelEnergyMap(unordered_map<int, double> *pixelEnergyMap)
{
   incrementTotalEnergiesToProcess();
   QMutexLocker locker(&mutex);
   pixelEnergyMapQueue.enqueue(pixelEnergyMap);
}

unordered_map<int, double> *HxtItem::getNextPixelEnergyMap()
{
   if (pixelEnergyMap != NULL)
   {
      //free(pixelEnergyMap);
   }

   if (pixelEnergyMapQueueNotEmpty())
   {
      QMutexLocker locker(&mutex);
      pixelEnergyMap = pixelEnergyMapQueue.dequeue();
   }

   return pixelEnergyMap;
}

bool HxtItem::pixelEnergyMapQueueNotEmpty()
{
   QMutexLocker locker(&mutex);
   return !pixelEnergyMapQueue.isEmpty();
}

int HxtItem::getPixelEnergyMapQueueSize()
{
   QMutexLocker locker(&mutex);
   return pixelEnergyMapQueue.size();
}

void HxtItem::setTotalEnergiesToProcess(long long totalEnergiesToProcess)
{
   QMutexLocker locker(&mutex);
   this->totalEnergiesToProcess = totalEnergiesToProcess;
}

void HxtItem::incrementTotalEnergiesToProcess()
{
   QMutexLocker locker(&mutex);
   this->totalEnergiesToProcess = totalEnergiesToProcess++;
}

long long HxtItem::getTotalEnergiesToProcess()
{
   QMutexLocker locker(&mutex);
   return totalEnergiesToProcess;
}

long long HxtItem::getBinStart() const
{
   return binStart;
}

void HxtItem::setBinStart(const long long value)
{
   binStart = value;
}

long long HxtItem::getBinEnd() const
{
   return binEnd;
}

void HxtItem::setBinEnd(const long long value)
{
   binEnd = value;
}

double HxtItem::getBinWidth() const
{
   return binWidth;
}

void HxtItem::setBinWidth(double value)
{
   binWidth = value;
}

void HxtItem::addToHistogram(unordered_map<int, double> pixelEnergyMap)
{
   double *currentHistogram = &histogramPerPixel[0];
   double thisEnergy;
   int bin;
   int pixel;

   unordered_map<int, double>::iterator it = pixelEnergyMap.begin();
   unordered_map<int, double>::iterator itend = pixelEnergyMap.end();

   while (it != itend)
   {
      pixel = it->first;
      thisEnergy = it->second;
      bin = (int)((thisEnergy / binWidth));
      (*(currentHistogram + (pixel * nBins) + bin))++;
      it++;
   }

   energiesProcessed++;
}

/*
void HxtItem::addToHistogramWithSum(unordered_map<int, double> pixelEnergyMap)
{

   double *currentHistogram = &histogramPerPixel[0];
   double *summed = &summedHistogram[0];
   double thisEnergy;
   int bin;
   int pixel;

   unordered_map<int, double>::iterator it = pixelEnergyMap.begin();
   unordered_map<int, double>::iterator itend = pixelEnergyMap.end();

   while (it != itend)
   {
      pixel = it->first;
      thisEnergy = it->second;
      bin = (int)((thisEnergy / binWidth));
      (*(currentHistogram + (pixel * nBins) + bin))++;
      (*(summed + b
      it++;
   }

   energiesProcessed++;
//   qDebug() << "HxtItem::addToHistogram(), energiesProcessed: " << energiesProcessed;
}
*/
HxtItem::HxtV3Buffer *HxtItem::getHxtV3Buffer()
{
   return &hxtV3Buffer;
}

double *HxtItem::getHxtV3AllData()
{
   return hxtV3Buffer.allData;
}

