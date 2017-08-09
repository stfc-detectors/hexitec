#include "hxttotalspectrumgenerator.h"

#include <QThread>
#include <QDebug>
#include <Windows.h>

/*
HxtTotalSpectrumGenerator::HxtTotalSpectrumGenerator()
{

}
*/

HxtTotalSpectrumGenerator::HxtTotalSpectrumGenerator(int frameSize, long long binStart, long long binEnd, double binWidth)  : GeneralHxtGenerator(frameSize, binStart, binEnd, binWidth)
{
//   int frameSize = 6400;
   hxtItem = new HxtItem(frameSize, binStart, binEnd, binWidth);

   hxtGeneratorThread = new QThread();
   hxtGeneratorThread->start();
   moveToThread(hxtGeneratorThread);

   processedEnergyCount = 0;
   hxtItem->setTotalEnergiesToProcess(0);
   connect(this, SIGNAL(process()), this, SLOT(handleProcess()));
   emit process();
}

HxtTotalSpectrumGenerator::~HxtTotalSpectrumGenerator()
{

}

void HxtTotalSpectrumGenerator::handleProcess()
{
   int temp1 = -1;
   int temp2 = -1;

   qDebug() << "HxtTotalSpectrumGenerator::handleProcess() called, running in thread." << QThread::currentThreadId();
   while (getFrameProcessingInProgress() || (hxtItem->getPixelEnergyQueueSize() > 0) || processedEnergyCount < (temp2 = hxtItem->getTotalEnergiesToProcess()))
   {
      while (getFrameProcessingInProgress() &&((temp1 = hxtItem->getPixelEnergyQueueSize()) == 0))
      {
         Sleep(10);
      }
      qDebug() << "GOT AN ENERGY TO PROCESS";
      while ((temp1 = hxtItem->getPixelEnergyQueueSize()) > 0)
      {
//         qDebug() <<"PixelEnergyQueueSize() = " << temp1  << "processedEnergyCount: " << processedEnergyCount
//                 << "totalEnergies to Process: " << hxtItem->getTotalEnergiesToProcess();
//         pixelEnergy = hxtItem->getNextPixelEnergy();
         hxtItem->getNextPixelEnergy();

        processedEnergyCount++;
        /*
        if (processedEnergyCount > 100)
        {
           setImageInProgress(false);
        }
        */
      }

   }
   qDebug() << "HxtGenerator::handleProcess() number of energies processed: " << processedEnergyCount
            << "totalEnergies to Process: " << temp2;
//   emit energyProcessingComplete(processedEnergyCount);
}
