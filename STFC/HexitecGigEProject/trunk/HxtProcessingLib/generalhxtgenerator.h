#ifndef GENERALHXTGENERATOR_H
#define GENERALHXTGENERATOR_H

#include "hxtitem.h"
#include <QObject>
#include <QDebug>
#include <QMutex>
#include <cstdint>

class GeneralHxtGenerator : public QObject
{
   Q_OBJECT

public:
//   GeneralHxtGenerator();
   GeneralHxtGenerator(int frameSize, long long binStart, long long binEnd, long long binWidth);
   void enqueuePixelEnergy(double *pixelEnergy);
   void setFrameProcessingInProgress(bool inProgress);
   void incrementProcessedEnergyCount();
   long long getProcessedEnergyCount();
   HxtItem::HxtV3Buffer *getHxtV3Buffer();
   double *getHxtV3AllData();

protected:
   bool getFrameProcessingInProgress();
   QThread *hxtGeneratorThread;
   QMutex mutex;
   HxtItem *hxtItem;
   long long *histogram;
   int frameSize;
   long long binStart;
   long long binEnd;
   long long binWidth;
//   long long totalEnergiesToProcess;
   long long processedEnergyCount;
   bool inProgress;
 
signals:
   void enqueue(double *pixelEnergy);
   void process();

public slots:
   virtual void handleProcess() = 0;
   void handleImageComplete();

};

#endif // GENERALHXTGENERATOR_H
