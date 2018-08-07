#include "hxtchargedsharingsumgenerator.h"

HxtChargedSharingSumGenerator::HxtChargedSharingSumGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition) :
   HxtChargedSharingGenerator(nRows, nCols, processingDefinition)
{

}

void HxtChargedSharingSumGenerator::processEnergies(double *frame)
{
   calculateChargedSharing(frame);
   hxtItem->addFrameDataToHistogramWithSum(frame);
   incrementProcessedEnergyCount();
}

