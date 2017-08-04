#include "generalhxtgenerator.h"
#include <QThread>
#include <QDebug>

//GeneralHxtGenerator::GeneralHxtGenerator()
GeneralHxtGenerator::GeneralHxtGenerator(int frameSize, long long binStart, long long binEnd, long long binWidth)
{
//   int frameSize = 6400;
   qDebug() << "GeneralHxtGenerator::GeneralHxtGenerator() called";
   hxtItem = new HxtItem(frameSize, binStart, binEnd, binWidth);

   hxtGeneratorThread = new QThread();
   hxtGeneratorThread->start();
   moveToThread(hxtGeneratorThread);

   processedEnergyCount = 0;
   hxtItem->setTotalEnergiesToProcess(0);
   connect(this, SIGNAL(process()), this, SLOT(handleProcess()));
   setFrameProcessingInProgress(true);
   emit process();
}

void GeneralHxtGenerator::enqueuePixelEnergy(double *pixelEnergy)
{
   hxtItem->enqueuePixelEnergy(pixelEnergy);
//   qDebug() << "GeneralHxtGenerator::enqueuePixelEnergy(), threadId: " << QThread::currentThreadId();
}


bool GeneralHxtGenerator::getFrameProcessingInProgress()
{
   QMutexLocker locker(&mutex);
   return inProgress;
}

void GeneralHxtGenerator::handleImageComplete()
{
   setFrameProcessingInProgress(false);
}

void GeneralHxtGenerator::setFrameProcessingInProgress(bool inProgress)
{
   QMutexLocker locker(&mutex);
   this->inProgress = inProgress;
}

void GeneralHxtGenerator::incrementProcessedEnergyCount()
{
   QMutexLocker locker(&mutex);
   this->processedEnergyCount = processedEnergyCount++;
}

long long GeneralHxtGenerator::getProcessedEnergyCount()
{
   QMutexLocker locker(&mutex);
   return processedEnergyCount;
}

HxtItem::HxtV3Buffer *GeneralHxtGenerator::getHxtV3Buffer()
{
   return hxtItem->getHxtV3Buffer();
}

double *GeneralHxtGenerator::getHxtV3AllData()
{
   return hxtItem->getHxtV3AllData();
}
