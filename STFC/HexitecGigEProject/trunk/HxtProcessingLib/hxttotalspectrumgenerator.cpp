#include "hxttotalspectrumgenerator.h"

#include <QThread>
#include <QDebug>
#include <Windows.h>

HxtTotalSpectrumGenerator::HxtTotalSpectrumGenerator(int frameSize, unsigned long long binStart, unsigned long long binEnd, unsigned long long binWidth) : GeneralHxtGenerator(frameSize, binStart, binEnd, binWidth)
{
   qDebug() << "Create a total spectrum";
   totalSpectrumItem = new HxtItem(frameSize, binStart, binEnd, binWidth);
   setImageInProgress(true);
//   connect(this, SIGNAL(process()), this, SLOT(handleProcess()));
//   emit process();

}

HxtTotalSpectrumGenerator::~HxtTotalSpectrumGenerator()
{

}

void HxtTotalSpectrumGenerator::handleProcess()
{
   double *pixelEnergy;

   qDebug() << "HxtTotalSpectrumGenerator::handleProcess() running in thread." << QThread::currentThreadId();;
   while (inProgress || (hxtItem->getPixelEnergyQueueSize() > 0))
   {
      while (inProgress &&((hxtItem->getPixelEnergyQueueSize() == 0)))
      {
         Sleep(10);
      }
      while (hxtItem->getPixelEnergyQueueSize() > 0)
      {
         pixelEnergy = hxtItem->getNextPixelEnergy();
         if (pixelEnergy != NULL)
         {
//               result = processEnergies(pixelEnergy);
               processEnergies(pixelEnergy);
               // MUST USE RESULT IN FURTHER CALCULATIONS
//               free(result);
//            writeFile(bufferStart, (validFrames * frameSize), filename);
//            free(bufferStart);
         }

      }
   }
}
void HxtTotalSpectrumGenerator::processEnergies(double *pixelEnergy)
{
   hxtItem->addToHistogram(pixelEnergy);
   //THIS COULD JUST ADD ALREADY CALCULATED HISTOGRAM TO TOTAL
   totalSpectrumItem->addToHistogram(pixelEnergy);
   qDebug() << "HxtGenerator::processEnergies called";
}
