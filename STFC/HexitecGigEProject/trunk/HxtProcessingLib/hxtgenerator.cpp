#include "hxtgenerator.h"

#include <QThread>
#include <QDebug>
#include <Windows.h>

HxtGenerator::HxtGenerator(int frameSize, long long binStart, long long binEnd, double binWidth)  : GeneralHxtGenerator(frameSize, binStart, binEnd, binWidth)
{
   qDebug() << "HxtGenerator::HxtGenerator() called";
}

void HxtGenerator::handleProcess()
{
   double *pixelEnergy;
   long long gotEnergy = 0;
   long long energyNotNull = 0;

   qDebug() << "HxtGenerator::handleProcess() called, running in thread." << QThread::currentThreadId();
   while (getFrameProcessingInProgress() || (hxtItem->getPixelEnergyQueueSize() > 0) || processedEnergyCount < (hxtItem->getTotalEnergiesToProcess()))
   {
      while (getFrameProcessingInProgress() &&((hxtItem->getPixelEnergyQueueSize()) == 0))
      {
         Sleep(10);
      }
      qDebug() << "GOT AN ENERGY TO PROCESS";
      while ((hxtItem->getPixelEnergyQueueSize()) > 0)
      {
         gotEnergy++;
         pixelEnergy = hxtItem->getNextPixelEnergy();
         if (pixelEnergy != NULL)
         {
            energyNotNull++;
//               result = processEnergies(pixelEnergy);

                 processEnergies(pixelEnergy);
//                 free(pixelEnergy);

               // MUST USE RESULT IN FURTHER CALCULATIONS
//               free(result);
//            writeFile(bufferStart, (validFrames * frameSize), filename);
//            free(bufferStart);
         }
      }
   }
   qDebug() << "HxtGenerator::handleProcess() number of energies processed: " << processedEnergyCount
            << "totalEnergies to Process: " << hxtItem->getTotalEnergiesToProcess();
   qDebug() << "queued energies =  " << gotEnergy
            << " energiesNotNull =  " << energyNotNull;
//   emit energyProcessingComplete(processedEnergyCount);
}

void HxtGenerator::processEnergies(double *pixelEnergy)
{
   hxtItem->addToHistogram(pixelEnergy);
   incrementProcessedEnergyCount();
}
