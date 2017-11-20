#ifndef HXTSUMGENERATOR_H
#define HXTSUMGENERATOR_H
#include "hxtgenerator.h"

class HxtSumGenerator : public HxtGenerator
{
public:
   HxtSumGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition);

   virtual void processEnergies(unordered_map<int, double> *pixelEnergyMap);
};

#endif // HXTSUMGENERATOR_H
