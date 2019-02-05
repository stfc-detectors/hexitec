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
                   uint16_t *frame, unsigned int *eventsInFrame);
   double *process(unordered_map<int, double>**pixelRawValMapPtr,
                   uint16_t *frame, uint16_t thresholdValue,
                   unsigned int *eventsInFrame);
   double *process(unordered_map<int, double>**pixelRawValMapPtr,
                   uint16_t *frame, uint16_t *thresholdPerPixel,
                   unsigned int *eventsInFrame);

   double *process(uint16_t *frame,
                   unordered_map<int, double>**pixelEnergyMapPtr,
                   unsigned int *eventsInFrame);
   double *process(uint16_t *frame, uint16_t thresholdValue,
                   unordered_map<int, double>**pixelEnergyMapPtr,
                   unsigned int *eventsInFrame);
   double *process(uint16_t *frame, uint16_t *thresholdPerPixel,
                   unordered_map<int, double>**pixelEnergyMapPtr,
                   unsigned int *eventsInFrame);

};

#endif // FRAMERE_ORDERPROCESSOR_H
