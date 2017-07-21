#include "hxtgenerator.h"

#include <QThread>
#include <QDebug>
#include <Windows.h>

HxtGenerator::HxtGenerator(int frameSize, unsigned long long binStart, unsigned long long binEnd, unsigned long long binWidth) : GeneralHxtGenerator(frameSize, binStart, binEnd, binWidth)
{
   setImageInProgress(true);
//   connect(this, SIGNAL(process()), this, SLOT(handleProcess()));
//   emit process();

}

void HxtGenerator::handleProcess()
{
   double *pixelEnergy;

   qDebug() << "******************HxtGenerator::handleProcess() running in thread." << QThread::currentThreadId();
   while (inProgress || (hxtItem->getPixelEnergyQueueSize() > 0))
   {
      while (inProgress &&((hxtItem->getPixelEnergyQueueSize() == 0)))
      {
         Sleep(10);
      }
      qDebug() << "GOT AN ENERGY TO PROCESS";
      while (hxtItem->getPixelEnergyQueueSize() > 0)
      {
         qDebug() << "11111";
         pixelEnergy = hxtItem->getNextPixelEnergy();
         qDebug() << "22222";

         if (pixelEnergy != NULL)
         {
            qDebug() << "PROCESS AN ENERGY!!!!!!!!!!!!!!! " ;

//               result = processEnergies(pixelEnergy);

                 processEnergies(pixelEnergy);

               // MUST USE RESULT IN FURTHER CALCULATIONS
//               free(result);
//            writeFile(bufferStart, (validFrames * frameSize), filename);
//            free(bufferStart);
         }
         else
         {
            qDebug() << "pixelEnergy is NULL!!!!!!!!!!!!!!! ";
         }
         qDebug() << "33333";

      }
      qDebug() << "44444";

   }
   qDebug() << "HxtGenerator::handleProcess() number of energies processed:" << processedEnergyCount;
   emit energyProcessingComplete(processedEnergyCount);

}

void HxtGenerator::processEnergies(double *pixelEnergy)
{
//   hxtItem->addToHistogram(pixelEnergy);
   processedEnergyCount++;
   qDebug() << "HxtGenerator::processEnergies called";
}
