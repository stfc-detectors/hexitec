#ifndef GENERALHXTGENERATOR_H
#define GENERALHXTGENERATOR_H

#include "hxtitem.h"
#include <QObject>
#include <QList>
#include <QDebug>
#include <cstdint>

class GeneralHxtGenerator : public QObject
{
   Q_OBJECT

public:
   GeneralHxtGenerator(int frameSize, unsigned long long binStart, unsigned long long binEnd, unsigned long long binWidth);
   //   QList <HxtItem *>hxtItemList;
   void setImageInProgress(bool inProgress);
   void enqueuePixelEnergy(double *pixelEnergy);

protected:
   unsigned int *histogram;
   HxtItem *hxtItem;
   int frameSize;
   unsigned long long binStart;
   unsigned long long binEnd;
   unsigned long long binWidth;

protected:
   QThread *hxtGeneratorThread;
   bool inProgress;
   virtual void processEnergies(double *pixelEnergy) = 0;
   unsigned long long totalEnergiesToProcess;
   unsigned long long processedEnergyCount;

signals:
   void process();
   void energyProcessingComplete(unsigned long long processedEnergyCount);

public slots:
   virtual void handleProcess() = 0;
   void imageComplete(unsigned long long totalEnergiesToProcess);

};

#endif // GENERALHXTGENERATOR_H
