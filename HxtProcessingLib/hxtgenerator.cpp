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
    hxtItem->addFrameDataToHistogram(frame, 1);
    incrementProcessedEnergyCount();
}

void HxtGenerator::processEnergies(double *frame)
{
    hxtItem->addFrameDataToHistogram(frame);
    incrementProcessedEnergyCount();
}
