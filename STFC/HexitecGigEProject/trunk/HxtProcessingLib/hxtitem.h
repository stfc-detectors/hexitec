#ifndef HXTITEM_H
#define HXTITEM_H

#include <QMutex>
#include <QQueue>
#include <cstdint>

class HxtItem
{
public:
   HxtItem(int frameSize, unsigned long long binStart, unsigned long long binEnd, unsigned long long binWidth);
   void enqueuePixelEnergy(double *pixelEnergy);
   double *getNextPixelEnergy();
   int getPixelEnergyQueueSize();
   void addToHistogram(double *pixelEnergy);

private:
   unsigned long long getBinStart() const;
   void setBinStart(const unsigned long long value);
   unsigned long long getBinEnd() const;
   void setBinEnd(const unsigned long long value);
   unsigned long long getBinWidth() const;
   void setBinWidth(const unsigned long long value);
//   void addToHistogram(double *pixelEnergy);
   bool pixelEnergyQueueNotEmpty();
   QMutex mutex;
   double *pixelEnergy;
   int frameSize;
   unsigned long long binStart;
   unsigned long long binEnd;
   unsigned long long binWidth;
   unsigned long long nBins;
   unsigned long *histogramPerPixel;
   unsigned long histogramIndex;
   unsigned long energiesProcessed;
   QQueue <double *>pixelEnergyQueue;
};

#endif // HXTITEM_H
