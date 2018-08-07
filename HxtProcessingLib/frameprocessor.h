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
   double *process(unordered_map<int, double>**pixelRawValMapPtr, uint16_t *frame);
   double *process(unordered_map<int, double>**pixelRawValMapPtr, uint16_t *frame,
                     uint16_t thresholdValue);
   double *process(unordered_map<int, double>**pixelRawValMapPtr,
                     uint16_t *frame, uint16_t *thresholdPerPixel);

   double *process(uint16_t *frame,
                     unordered_map<int, double>**pixelEnergyMapPtr);
   double *process(uint16_t *frame, uint16_t thresholdValue,
                     unordered_map<int, double>**pixelEnergyMapPtr);
   double *process(uint16_t *frame, uint16_t *thresholdPerPixel,
                     unordered_map<int, double>**pixelEnergyMapPtr);
};

#endif // FRAMEPROCESSOR_H
