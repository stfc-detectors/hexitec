#ifndef FRAMERE_ORDERPROCESSOR_H
#define FRAMERE_ORDERPROCESSOR_H

#include "generalframeprocessor.h"

using namespace std;

class FrameRe_orderProcessor : public GeneralFrameProcessor
{
public:
   FrameRe_orderProcessor(bool nextFrameCorrection);
   ~FrameRe_orderProcessor();
   uint16_t *process(unordered_map<int, double> **pixelRawValMapPtr, 
                     uint16_t *frame);
   uint16_t *process(unordered_map<int, double>**pixelRawValMapPtr, 
                     uint16_t *frame, uint16_t thresholdValue);
   uint16_t *process(unordered_map<int, double>**pixelRawValMapPtr, 
                     uint16_t *frame, uint16_t *thresholdPerPixel);

   uint16_t *process(uint16_t *frame,
                     unordered_map<int, double>**pixelEnergyMapPtr);
   uint16_t *process(uint16_t *frame, uint16_t thresholdValue,
                     unordered_map<int, double>**pixelEnergyMapPtr);
   uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel,
                     unordered_map<int, double>**pixelEnergyMapPtr);

};

#endif // FRAMERE_ORDERPROCESSOR_H
