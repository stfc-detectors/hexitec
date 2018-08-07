#include "hxtsumgenerator.h"

HxtSumGenerator::HxtSumGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition) :
   HxtGenerator(nRows, nCols, processingDefinition)
{

}

void HxtSumGenerator::processEnergies(double *frame)
{
   hxtItem->addFrameDataToHistogramWithSum(frame);
   incrementProcessedEnergyCount();
}
