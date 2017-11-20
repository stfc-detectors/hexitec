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
   FrameProcessor(bool nextFrameCorrection);
   ~FrameProcessor();
   uint16_t *process(unordered_map<int, double>**pixelRawValMapPtr, uint16_t *frame);
   uint16_t *process(unordered_map<int, double>**pixelRawValMapPtr, uint16_t *frame, 
                     uint16_t thresholdValue);
   uint16_t *process(unordered_map<int, double>**pixelRawValMapPtr, 
                     uint16_t *frame, uint16_t *thresholdPerPixel);

   uint16_t *process(uint16_t *frame,
                     unordered_map<int, double>**pixelEnergyMapPtr);
   uint16_t *process(uint16_t *frame, uint16_t thresholdValue,
                     unordered_map<int, double>**pixelEnergyMapPtr);
   uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel,
                     unordered_map<int, double>**pixelEnergyMapPtr);
};

#endif // FRAMEPROCESSOR_H
