#ifndef HXTCHARGEDSHARINGGENERATOR_H
#define HXTCHARGEDSHARINGGENERATOR_H

#include "hxtgenerator.h"

class HxtChargedSharingGenerator : public HxtGenerator
{
public:
   HxtChargedSharingGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition);
   void setPixelGridSize(int pixelGridSize);

   virtual void processEnergies(unordered_map<int, double> *pixelEnergyMap);
   /// Trial doing Calibration & CS algorithm jointly
   virtual void calibrateAndApplyChargedAlgorithm(uint16_t *frame, uint16_t thresholdValue, double *gradients, double *intercepts);
   ///

protected:
  void calculateChargedSharing(unordered_map <int, double>*pixelEnergyMap);
  uint16_t *calibrateAndChargedSharing(uint16_t *frame, uint16_t thresholdValue, double *gradients, double *intercepts);

private:
  void processAdditionChargedSharing(unordered_map <int, double>*pixelEnergyMap, int length);
  void processDiscriminationChargedSharing(unordered_map <int, double>*pixelEnergyMap, int length);
  void setChargedSharingMode(ChargedSharingMode chargedSharingMode);
  ///
//  void processDCS(uint16_t *frame);
  ///
  int pixelGridSize;
  int directionalDistance;
  long long **pixelGrid;
  double maxCharge;
  double maxValue;
  int maxIndex;
  ChargedSharingMode chargedSharingMode;

};

#endif // HXTCHARGEDSHARINGGENERATOR_H
