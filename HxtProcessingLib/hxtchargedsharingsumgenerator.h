#ifndef HXTCHARGEDSHARINGSUMGENERATOR_H
#define HXCHARGEDSHARINGTSUMGENERATOR_H
#include "hxtchargedsharinggenerator.h"

class HxtChargedSharingSumGenerator : public HxtChargedSharingGenerator
{
public:
   HxtChargedSharingSumGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition);
   virtual void processEnergies(uint16_t *frame);
};

#endif // HXTCHARGEDSHARINGSUMGENERATOR_H
