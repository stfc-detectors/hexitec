#ifndef FRAMERE_ORDERPROCESSOR_H
#define FRAMERE_ORDERPROCESSOR_H

#include "generalframeprocessor.h"

using namespace std;

class FrameRe_orderProcessor : public GeneralFrameProcessor
{
public:
   FrameRe_orderProcessor();
   ~FrameRe_orderProcessor();
   uint16_t *process(uint16_t *frame);
   uint16_t *process(uint16_t *frame, uint16_t thresholdValue);
   uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel);
   uint16_t *process(uint16_t *frame, double **pixelEnergyPtr);
   uint16_t *process(uint16_t *frame, uint16_t thresholdValue, double **pixelEnergyPtr);
   uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel, double **pixelEnergyPtr);
};

#endif // FRAMERE_ORDERPROCESSOR_H
