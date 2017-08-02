#include "hxtitem.h"
#include <QMutexLocker>
#include <QDebug>
#include <QThread>
#include <iostream>

using namespace std;

HxtItem::HxtItem(int frameSize, long long binStart, long long binEnd, long long binWidth)
{
   this->frameSize = frameSize;
   setBinStart(binStart);
   setBinEnd(binEnd);
   setBinWidth(binWidth);
   nBins = (int)(((binEnd - binStart) / binWidth) + 0.5);
   histogramPerPixel = (unsigned long *) calloc(nBins * frameSize, sizeof(unsigned long));
   qDebug() << "HxtItem::HxtItem(): nBins = " << nBins << " frameSize = " << frameSize;
   pixelEnergy = NULL;
   energiesProcessed = 0;
   this->pixelEnergyQueue.clear();
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

long long HxtItem::getBinWidth() const
{
   return binWidth;
}

void HxtItem::setBinWidth(const long long value)
{
   binWidth = value;
}

void HxtItem::addToHistogram(double *pixelEnergy)
{
   unsigned long *currentHistogram = &histogramPerPixel[0];
   double *thisEnergy;
   int bin;

   for (int i = 0; i < frameSize; i++)
   {
      thisEnergy = pixelEnergy+i;
      if (*thisEnergy  > MIN_ENERGY)
      {
         bin = (int)((*thisEnergy / binWidth) + 0.5);
         if (bin < nBins)
         {
            (*(currentHistogram + bin))++;
         }
      }
      currentHistogram+=nBins;
   }

   energiesProcessed++;
//   qDebug() << "HxtItem::addToHistogram(), energiesProcessed: " << energiesProcessed;
}

