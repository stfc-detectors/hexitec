#ifndef HXTGENERATOR_H
#define HXTGENERATOR_H

#include "generalhxtgenerator.h"
#include <unordered_map>

using namespace std;

class HxtGenerator : public GeneralHxtGenerator
{
public:
   HxtGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition);

   virtual void processEnergies(uint16_t *frame);
};

#endif // HXTGENERATOR_H
