#ifndef HXTCHARGEDSHARINGGENERATOR_H
#define HXTCHARGEDSHARINGGENERATOR_H

#include "hxtgenerator.h"

class HxtChargedSharingGenerator : public HxtGenerator
{
public:
   HxtChargedSharingGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition);
   void setPixelGridSize(int pixelGridSize);

   virtual void processEnergies(uint16_t *frame);

protected:
  void calculateChargedSharing(uint16_t *frame);

private:
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
