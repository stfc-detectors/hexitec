#ifndef HXTGENERATOR_H
#define HXTGENERATOR_H

#include "generalhxtgenerator.h"
#include <unordered_map>

using namespace std;

class HxtGenerator : public GeneralHxtGenerator
{
public:
   HxtGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition);

protected:
   virtual void processEnergies(unordered_map<int, double> *pixelEnergyMap);
   virtual void processEnergiesWithSum(unordered_map<int, double> *pixelEnergyMap);

public slots:
  void handleProcess();
  void handleProcess(bool totalSpectrum);
};

#endif // HXTGENERATOR_H
