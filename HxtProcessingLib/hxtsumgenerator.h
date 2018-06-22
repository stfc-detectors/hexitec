#ifndef HXTSUMGENERATOR_H
#define HXTSUMGENERATOR_H
#include "hxtgenerator.h"

class HxtSumGenerator : public HxtGenerator
{
public:
   HxtSumGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition);

   virtual void processEnergies(uint16_t *frame);
};

#endif // HXTSUMGENERATOR_H
