#include "hxtsumgenerator.h"
///
#include <QDebug>
HxtSumGenerator::HxtSumGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition) :
   HxtGenerator(nRows, nCols, processingDefinition)
{

}

void HxtSumGenerator::processEnergies(unordered_map <int, double>*pixelEnergyMap)
{
   hxtItem->addToHistogramWithSum(*pixelEnergyMap);
   incrementProcessedEnergyCount();
   delete pixelEnergyMap;
}

void HxtSumGenerator::calibrateAndApplyChargedAlgorithm(uint16_t *frame, uint16_t thresholdValue, double *gradients, double *intercepts)
{
   qDebug() << "HxtSumGenerator::calibrateAndApplyChargedAlgorithm(" << frame << thresholdValue << gradients << intercepts;
}
