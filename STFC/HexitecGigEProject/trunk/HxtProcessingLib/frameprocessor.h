#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

#include "generalframeprocessor.h"

#include <string>
#include <cstdint>
#include <list>
#include <regex>

using namespace std;

class FrameProcessor : public GeneralFrameProcessor
{

public:
   FrameProcessor();
   ~FrameProcessor();
   uint16_t *process(uint16_t *frame);
   uint16_t *process(uint16_t *frame, uint16_t thresholdValue);
   uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel);
   uint16_t *process(uint16_t *frame, double **pixelEnergyPtr);
   uint16_t *process(uint16_t *frame, uint16_t thresholdValue, double **pixelEnergyPtr);
   uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel, double **pixelEnergyPtr);
};

#endif // FRAMEPROCESSOR_H
