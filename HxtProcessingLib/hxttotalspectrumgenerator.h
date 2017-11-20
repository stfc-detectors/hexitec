#ifndef HXTTOTALSPECTRUMGENERATOR_H
#define HXTTOTALSPECTRUMGENERATOR_H

#include "generalhxtgenerator.h"
#include <unordered_map>

using namespace std;

class HxtTotalSpectrumGenerator : public GeneralHxtGenerator
{
public:
   HxtTotalSpectrumGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition);
   void processEnergies(unordered_map<int, double> *pixelEnergyMap);
   void processEnergiesWithSum(unordered_map<int, double> *pixelEnergyMap);

public slots:
   void handleProcess();
};

#endif // HXTTOTALSPECTRUMGENERATOR_H
