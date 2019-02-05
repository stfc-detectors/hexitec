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
   FrameProcessor(bool nextFrameCorrection, int occupancyThreshold);
   ~FrameProcessor();
   double *process(unordered_map<int, double>**pixelRawValMapPtr, uint16_t *frame,
                   unsigned int *eventsInFrame);
   double *process(unordered_map<int, double>**pixelRawValMapPtr, uint16_t *frame,
                   uint16_t thresholdValue, unsigned int *eventsInFrame);
   double *process(unordered_map<int, double>**pixelRawValMapPtr,
                   uint16_t *frame, uint16_t *thresholdPerPixel,
                   unsigned int *eventsInFrame);

   double *process(uint16_t *frame,
                   unordered_map<int, double>**pixelEnergyMapPtr, unsigned int *eventsInFrame);
   double *process(uint16_t *frame, uint16_t thresholdValue,
                   unordered_map<int, double>**pixelEnergyMapPtr, unsigned int *eventsInFrame);
   double *process(uint16_t *frame, uint16_t *thresholdPerPixel,
                   unordered_map<int, double>**pixelEnergyMapPtr, unsigned int *eventsInFrame);
};

#endif // FRAMEPROCESSOR_H
