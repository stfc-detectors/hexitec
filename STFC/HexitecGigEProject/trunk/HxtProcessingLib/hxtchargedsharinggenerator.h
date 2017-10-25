#ifndef HXTCHARGEDSHARINGGENERATOR_H
#define HXTCHARGEDSHARINGGENERATOR_H

#include "hxtgenerator.h"

class HxtChargedSharingGenerator : public HxtGenerator
{
public:
   HxtChargedSharingGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition);
   void setPixelGridSize(int pixelGridSize);

   virtual void processEnergies(unordered_map<int, double> *pixelEnergyMap);

private:
  void calculateChargedSharing(unordered_map <int, double>*pixelEnergyMap);
  void processAdditionChargedSharing(unordered_map <int, double>*pixelEnergyMap, int length);
  void processDiscriminationChargedSharing(unordered_map <int, double>*pixelEnergyMap, int length);
  void setChargedSharingMode(ChargedSharingMode chargedSharingMode);
  int pixelGridSize;
  int directionalDistance;
  long long **pixelGrid;
  double maxCharge;
  int *pixelRow;
  int *pixelCol;
  double *pixelValue;
  double maxValue;
  int maxIndex;
  ChargedSharingMode chargedSharingMode;

};

#endif // HXTCHARGEDSHARINGGENERATOR_H
