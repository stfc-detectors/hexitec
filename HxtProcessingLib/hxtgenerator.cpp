#include "hxtgenerator.h"

#include <Windows.h>

HxtGenerator::HxtGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition) :
   GeneralHxtGenerator(nRows, nCols, processingDefinition)
{
   if (processingDefinition->getTotalSpectrum())
   {
      hxtItem->initialiseTotalSpectrum();
   }
}

void HxtGenerator::processEnergies(unordered_map <int, double>*pixelEnergyMap)
{
   hxtItem->addToHistogram(*pixelEnergyMap);
   incrementProcessedEnergyCount();
   delete pixelEnergyMap;
}


