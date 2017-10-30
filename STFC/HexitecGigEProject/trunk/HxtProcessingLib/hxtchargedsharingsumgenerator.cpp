#include "hxtchargedsharingsumgenerator.h"

HxtChargedSharingSumGenerator::HxtChargedSharingSumGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition) :
   HxtChargedSharingGenerator(nRows, nCols, processingDefinition)
{

}

void HxtChargedSharingSumGenerator::processEnergies(unordered_map <int, double>*pixelEnergyMap)
{
   calculateChargedSharing(pixelEnergyMap);
   hxtItem->addToHistogramWithSum(*pixelEnergyMap);
   incrementProcessedEnergyCount();
   delete pixelEnergyMap;
}
