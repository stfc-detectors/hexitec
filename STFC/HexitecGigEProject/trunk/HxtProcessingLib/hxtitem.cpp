#include "hxtitem.h"
#include <QMutexLocker>
#include <QDebug>
#include <QThread>
#include <iostream>

using namespace std;

HxtItem::HxtItem(int frameSize, unsigned long long binStart, unsigned long long binEnd, unsigned long long binWidth)
{
//   QMutexLocker locker(&mutex);
   this->frameSize = frameSize;
   setBinStart(binStart);
   setBinEnd(binEnd);
   setBinWidth(binWidth);
   nBins = (int)(((binEnd - binStart) / binWidth) + 0.5);
   histogramPerPixel = (unsigned long *) calloc(nBins * frameSize, sizeof(unsigned long));
   pixelEnergy = NULL;
   energiesProcessed = 0;
   this->pixelEnergyQueue.clear();
   qDebug() << "NEW HxtItem Created, histogram size = nBins: " << nBins;
}

void HxtItem::enqueuePixelEnergy(double *pixelEnergy)
{
   QMutexLocker locker(&mutex);
   pixelEnergyQueue.enqueue(pixelEnergy);
   qDebug() << "HxtItem::enqueuePixelEnergy() called, thread = " << QThread::currentThreadId();
}

double *HxtItem::getNextPixelEnergy()
{
   if (pixelEnergy != NULL)
   {
      //free(pixelEnergy);
   }

   if (pixelEnergyQueueNotEmpty())
   {
      qDebug() << "HxtItem::getNextPixelEnergy called. Queue length = " << getPixelEnergyQueueSize()
               << "thread = " << QThread::currentThreadId();
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

unsigned long long HxtItem::getBinStart() const
{
   return binStart;
}

void HxtItem::setBinStart(const unsigned long long value)
{
   binStart = value;
}

unsigned long long HxtItem::getBinEnd() const
{
   return binEnd;
}

void HxtItem::setBinEnd(const unsigned long long value)
{
   binEnd = value;
}

unsigned long long HxtItem::getBinWidth() const
{
   return binWidth;
}

void HxtItem::setBinWidth(const unsigned long long value)
{
   binWidth = value;
}

void HxtItem::addToHistogram(double *pixelEnergy)
{
   unsigned long *currentHistogram = histogramPerPixel;
   double *thisEnergy;
   int bin;

   qDebug() <<"HxtItem::addToHistogram() running in thread." << QThread::currentThreadId();
   for (int i = 0; i < frameSize; i++)
   {
//      qDebug() <<"++++++++++++++++++++++++++++++++++++++++++++++++++";
      thisEnergy = pixelEnergy+i;
      if (*thisEnergy  != 0)
      {
//         bin = (int)((pixelEnergy[i] / binWidth) + 0.5);
         bin = (int)((*thisEnergy / binWidth) + 0.5);
         if (bin < nBins)
         {
            (*(currentHistogram + bin))++;
//            qDebug() << "incrementing histogram !!!: " << bin << ", "<< pixelEnergy[i];
         }
         else
         {
//            qDebug() << "histogram index out of range!!!: " << bin << ", "<< pixelEnergy[i];
         }
      }
      currentHistogram+=nBins;
   }
   energiesProcessed++;
   qDebug() << "HxtItem::addToHistogram(), energiesProcessed: " << energiesProcessed;
}

