#ifndef HXTITEM_H
#define HXTITEM_H

#include <QMutex>
#include <QQueue>
#include <cstdint>

#define MIN_ENERGY 1.0e-100

class HxtItem
{
public:
   HxtItem(int frameSize, long long binStart, long long binEnd, long long binWidth);
   void enqueuePixelEnergy(double *pixelEnergy);
   double *getNextPixelEnergy();
   int getPixelEnergyQueueSize();
   void setTotalEnergiesToProcess(long long totalEnergiesToProcess);
   void incrementTotalEnergiesToProcess();
   long long getTotalEnergiesToProcess();
   void addToHistogram(double *pixelEnergy);

private:
   long long getBinStart() const;
   void setBinStart(const long long value);
   long long getBinEnd() const;
   void setBinEnd(const long long value);
   long long getBinWidth() const;
   void setBinWidth(const long long value);
//   void addToHistogram(double *pixelEnergy);
   bool pixelEnergyQueueNotEmpty();
   QMutex mutex;
   double *pixelEnergy;
   int frameSize;
   long long binStart;
   long long binEnd;
   long long binWidth;
   long long nBins;
   unsigned long *histogramPerPixel;
   unsigned long histogramIndex;
   long long totalEnergiesToProcess;
   long long energiesProcessed;
   QQueue <double *>pixelEnergyQueue;
};

#endif // HXTITEM_H
