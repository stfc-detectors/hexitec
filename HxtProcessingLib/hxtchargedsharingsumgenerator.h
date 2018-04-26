#ifndef HXTCHARGEDSHARINGSUMGENERATOR_H
#define HXCHARGEDSHARINGTSUMGENERATOR_H
#include "hxtchargedsharinggenerator.h"

class HxtChargedSharingSumGenerator : public HxtChargedSharingGenerator
{
public:
   HxtChargedSharingSumGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition);
   virtual void processEnergies(unordered_map<int, double> *pixelEnergyMap);
   ///
   virtual void calibrateAndApplyChargedAlgorithm(uint16_t *frame, uint16_t thresholdValue, double *gradients, double *intercepts);
};

#endif // HXTCHARGEDSHARINGSUMGENERATOR_H
