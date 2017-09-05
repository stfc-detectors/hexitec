#include "generalhxtgenerator.h"
#include <QThread>
#include <QDebug>

GeneralHxtGenerator::GeneralHxtGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition)
{
   this->nRows = nRows;
   this->nCols = nCols;
   frameSize = nRows * nCols;
   hxtItem = new HxtItem(nRows, nCols, processingDefinition->getBinStart(), processingDefinition->getBinEnd(), processingDefinition->getBinWidth());

   hxtGeneratorThread = new QThread();
   hxtGeneratorThread->start();
   moveToThread(hxtGeneratorThread);

   processedEnergyCount = 0;
   hxtItem->setTotalEnergiesToProcess(0);
   connect(this, SIGNAL(process()), this, SLOT(handleProcess()));
   connect(this, SIGNAL(process(bool)), this, SLOT(handleProcess(bool)));
   setFrameProcessingInProgress(true);
}

GeneralHxtGenerator::~GeneralHxtGenerator()
{
   qDebug() << "GeneralHxtGenerator::~GeneralHxtGenerator()";
   delete hxtItem;
}

void GeneralHxtGenerator::enqueuePixelEnergy(double *pixelEnergy)
{
   hxtItem->enqueuePixelEnergy(pixelEnergy);
}

void GeneralHxtGenerator::enqueuePixelEnergyMap(unordered_map <int, double> *pixelEnergyMap)
{
   hxtItem->enqueuePixelEnergyMap(pixelEnergyMap);
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

double *GeneralHxtGenerator::getEnergyBin()
{
   QMutexLocker locker(&mutex);
   return hxtItem->getEnergyBin();
}

long long *GeneralHxtGenerator::getSummedHistogram()
{
   QMutexLocker locker(&mutex);
   return hxtItem->getSummedHistogram();
}


