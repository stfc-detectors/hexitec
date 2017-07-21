#include "generalhxtgenerator.h"
#include <QThread>
#include <QDebug>

GeneralHxtGenerator::GeneralHxtGenerator(int frameSize, unsigned long long binStart, unsigned long long binEnd, unsigned long long binWidth)
{
   hxtGeneratorThread = new QThread();
   hxtGeneratorThread->start();
   moveToThread(hxtGeneratorThread);

   hxtItem = new HxtItem(frameSize, binStart, binEnd, binWidth);
   processedEnergyCount = 0;
   totalEnergiesToProcess = 65535;
   connect(this, SIGNAL(process()), this, SLOT(handleProcess()));
   emit process();
}


void GeneralHxtGenerator::enqueuePixelEnergy(double *pixelEnergy)
{
   hxtItem->enqueuePixelEnergy(pixelEnergy);
}

void GeneralHxtGenerator::imageComplete(unsigned long long totalEnergiesToProcess)
{
   this->totalEnergiesToProcess = totalEnergiesToProcess;
   qDebug() <<"GeneralHxtGenerator::imageComplete(): " << totalEnergiesToProcess;
//   if (processedEnergyCount)
   setImageInProgress(false);
}

void GeneralHxtGenerator::setImageInProgress(bool inProgress)
{
   qDebug() << "================GeneralHxtGenerator::setImageInProgress(): " << inProgress;
   this->inProgress = inProgress;
}

/*
void GeneralHxtGenerator::processEnergies(double *pixelEnergy)
{
   qDebug() << "================GeneralHxtGenerator::processEnergies called";
}
*/
