#include "hxtgenerator.h"

#include <Windows.h>
///
#include <QDebug>

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

void HxtGenerator::calibrateAndApplyChargedAlgorithm(uint16_t *frame, uint16_t thresholdValue, double *gradients, double *intercepts)
{
   qDebug() << "HxtGenerator::calibrateAndApplyChargedAlgorithm(" << frame << thresholdValue << gradients << intercepts;
}


