#ifndef GENERALHXTGENERATOR_H
#define GENERALHXTGENERATOR_H

#include "hxtitem.h"
#include "processingdefinition.h"
#include <QObject>
#include <QDebug>
#include <QMutex>
#include <cstdint>
#include <unordered_map>

class GeneralHxtGenerator : public QObject
{
   Q_OBJECT

public:
   GeneralHxtGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition);
   void enqueuePixelEnergy(double *pixelEnergy);
   void enqueuePixelEnergyMap(unordered_map<int, double> *pixelEnergyMap);
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
   int nRows;
   int nCols;
   int frameSize;
//   long long binStart;
//   long long binEnd;
//   double binWidth;
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
