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
  void processDiscriminationRewritten(uint16_t *extendedFrame, int extendedFrameRows, int startPosn, int endPosn);
  void processAdditionRewritten(uint16_t *extendedFrame, int extendedFrameRows, int startPosn, int endPosn);
  void setChargedSharingMode(ChargedSharingMode chargedSharingMode);
  /// Debug function(s)
  void showFrameSubset(uint16_t *frame, int offset);
  void showCsdFrameSubset(uint16_t  *extendedFrame, int offset);
  void showCsdFrameBinContents(int extendedFrameSize, uint16_t *extendedFrame, int bin);
  void showFrameBinContents(int frameSize, uint16_t *frame, int bin);
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
