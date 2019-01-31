#ifndef FRAMERE_ORDERPROCESSOR_H
#define FRAMERE_ORDERPROCESSOR_H

#include "generalframeprocessor.h"

using namespace std;

class FrameRe_orderProcessor : public GeneralFrameProcessor
{
public:
   FrameRe_orderProcessor(bool nextFrameCorrection, int occupancyThreshold);
   ~FrameRe_orderProcessor();
   double *process(unordered_map<int, double> **pixelRawValMapPtr,
                     uint16_t *frame);
   double *process(unordered_map<int, double>**pixelRawValMapPtr,
                     uint16_t *frame, uint16_t thresholdValue);
   double *process(unordered_map<int, double>**pixelRawValMapPtr,
                     uint16_t *frame, uint16_t *thresholdPerPixel);

   double *process(uint16_t *frame,
                     unordered_map<int, double>**pixelEnergyMapPtr);
   double *process(uint16_t *frame, uint16_t thresholdValue,
                     unordered_map<int, double>**pixelEnergyMapPtr);
   double *process(uint16_t *frame, uint16_t *thresholdPerPixel,
                     unordered_map<int, double>**pixelEnergyMapPtr);

};

#endif // FRAMERE_ORDERPROCESSOR_H
