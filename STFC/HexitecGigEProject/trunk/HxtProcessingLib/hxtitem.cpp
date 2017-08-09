#include "hxtitem.h"
#include <QMutexLocker>
#include <QDebug>
#include <QThread>
#include <iostream>
#include <math.h>

using namespace std;


HxtItem::HxtItem(int frameSize, long long binStart, long long binEnd, double binWidth)
{
   this->frameSize = frameSize;
   setBinStart(binStart);
   setBinEnd(binEnd);
   setBinWidth(binWidth);
   nBins = (int)(((binEnd - binStart) / binWidth) + 0.5);
   initialiseHxtBuffer(frameSize);
//   bin = (unsigned long *) calloc(nBins, sizeof(unsigned long));
//   histogramPerPixel = (unsigned long *) calloc(nBins * frameSize, sizeof(unsigned long));
   qDebug() << "HxtItem::HxtItem(): nBins = " << nBins << " frameSize = " << frameSize;
   pixelEnergy = NULL;
   energiesProcessed = 0;
   this->pixelEnergyQueue.clear();
}

void HxtItem::initialiseHxtBuffer(int frameSize)
{
   quint32 gridSize;
   gridSize = (int) sqrt(frameSize);
   double currentBin;

   strncpy(hxtV3Buffer.hxtLabel, "HEXITECH", sizeof(hxtV3Buffer.hxtLabel));
   hxtV3Buffer.hxtVersion = 3;
   hxtV3Buffer.filePrefixLength = 0;
   strncpy(hxtV3Buffer.filePrefix, "(blank)", 7);
//   hxtV3Buffer.filePrefix = "(blank)             ";
   hxtV3Buffer.nRows = gridSize;
   hxtV3Buffer.nCols = gridSize;
   hxtV3Buffer.nBins = nBins;
   hxtV3Buffer.allData = (double *) calloc((nBins * frameSize) + nBins, sizeof(double));
   energyBin = hxtV3Buffer.allData;
   histogramPerPixel = hxtV3Buffer.allData + nBins;
   qDebug() << "HxtItem::initialiseHxtBuffer, nRows: " << hxtV3Buffer.nRows << " nCols: " << hxtV3Buffer.nCols
            << " nBins: " << hxtV3Buffer.nBins;

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

void HxtItem::addToHistogram(double *pixelEnergy)
{
   double *currentHistogram = &histogramPerPixel[0];
   double *thisEnergy;
   int bin;

   qDebug() << "frameSize = " << frameSize << " nBins: " << nBins << " binWidth: " << binWidth;
   for (int i = 0; i < frameSize; i++)
   {
      thisEnergy = pixelEnergy+i;
      if (*thisEnergy  > MIN_ENERGY)
      {
         bin = (int)((*thisEnergy / binWidth));
//         bin = (int)((*thisEnergy / binWidth) + 0.5);

//         if (bin < nBins && bin > 0)
         if (bin < nBins)
        {
            (*(currentHistogram + bin))++;
//            qDebug() << "bin: " << bin << " energy: " << *thisEnergy;
         }
      }
      else
      {
//         qDebug() << "!!!!!!!!!!!!!!! ZERO ENERGY";
      }
      currentHistogram+=nBins;
   }

   energiesProcessed++;
   qDebug() << "HxtItem::addToHistogram(), energiesProcessed: " << energiesProcessed;
}

HxtItem::HxtV3Buffer *HxtItem::getHxtV3Buffer()
{
   return &hxtV3Buffer;
}

double *HxtItem::getHxtV3AllData()
{
   return hxtV3Buffer.allData;
}

