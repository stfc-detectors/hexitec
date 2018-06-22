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

void HxtGenerator::processEnergies(uint16_t *frame)
{
    hxtItem->addFrameDataToHistogram(frame, thresholdValue);
    incrementProcessedEnergyCount();
}
