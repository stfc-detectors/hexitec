#include "hxttotalspectrumgenerator.h"

#include <QThread>
#include <QDebug>
#include <Windows.h>

HxtTotalSpectrumGenerator::HxtTotalSpectrumGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition)  :
   GeneralHxtGenerator(nRows, nCols, processingDefinition)
{
   qDebug() << "HxtTotalSpectrumGenerator::HxtTotalSpectrumGenerator CONSTRUCTED";
   if (processingDefinition->getTotalSpectrum())
   {
      hxtItem->initialiseTotalSpectrum();
   }
   emit process();
}

void HxtTotalSpectrumGenerator::handleProcess()
{

   unordered_map <int, double> *pixelEnergyMap;
   int temp = 0;

  while (getFrameProcessingInProgress() || (hxtItem->getPixelEnergyMapQueueSize() > 0) || processedEnergyCount < (hxtItem->getTotalEnergiesToProcess()))
   {
      while (getFrameProcessingInProgress() &&(((temp = hxtItem->getPixelEnergyMapQueueSize())) == 0))
      {
         Sleep(10);
      }
//      qDebug() << "NUMBER OF ENERGIES TO PROCESS: " << temp;
      while ((hxtItem->getPixelEnergyMapQueueSize()) > 0)
      {
         pixelEnergyMap = hxtItem->getNextPixelEnergyMap();
         if (!pixelEnergyMap->empty())
         {
//               result = processEnergies(pixelEnergy);

                 processEnergies(*pixelEnergyMap);
//                 free(pixelEnergy);

               // MUST USE RESULT IN FURTHER CALCULATIONS
//               free(result);
//            writeFile(bufferStart, (validFrames * frameSize), filename);
//            free(bufferStart);
         }
      }
   }
//   emit energyProcessingComplete(processedEnergyCount);
}

void HxtTotalSpectrumGenerator::processEnergies(unordered_map <int, double>pixelEnergyMap)
{
   hxtItem->addToHistogramWithSum(pixelEnergyMap);
   incrementProcessedEnergyCount();
}
