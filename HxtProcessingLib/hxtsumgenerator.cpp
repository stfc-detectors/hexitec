#include "hxtsumgenerator.h"

HxtSumGenerator::HxtSumGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition) :
   HxtGenerator(nRows, nCols, processingDefinition)
{

}

void HxtSumGenerator::processEnergies(uint16_t *frame)
{
   hxtItem->addFrameDataToHistogramWithSum(frame, thresholdValue);
   incrementProcessedEnergyCount();
}
