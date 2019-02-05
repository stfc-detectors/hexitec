#include "frameprocessor.h"
#include <iostream>
#include <fstream>

FrameProcessor::FrameProcessor(bool nextFrameCorrection, int occupancyThreshold) :
   GeneralFrameProcessor(nextFrameCorrection, occupancyThreshold)
{
}


double *FrameProcessor::process(unordered_map<int, double> **pixelRawValMapPtr,
                                uint16_t *frame, unsigned int *eventsInFrame)
{
   return pixelProcessor->processFrame(pixelRawValMapPtr, frame, eventsInFrame);
}


double *FrameProcessor::process(unordered_map<int, double>**pixelRawValMapPtr,
                                uint16_t *frame, uint16_t thresholdValue,
                                unsigned int *eventsInFrame)
{
   return pixelProcessor->processFrame(pixelRawValMapPtr, frame, thresholdValue,
                                       eventsInFrame);
}


double *FrameProcessor::process(unordered_map<int, double>**pixelRawValMapPtr,
                                uint16_t *frame, uint16_t *thresholdPerPixel,
                                unsigned int *eventsInFrame)
{
   return pixelProcessor->processFrame(pixelRawValMapPtr, frame, thresholdPerPixel,
                                       eventsInFrame);
}


double *FrameProcessor::process(uint16_t *frame,
                                unordered_map<int, double>**pixelEnergyMapPtr,
                                unsigned int *eventsInFrame)
{
   return pixelProcessor->processFrame(frame, pixelEnergyMapPtr, eventsInFrame);
}


double *FrameProcessor::process(uint16_t *frame, uint16_t thresholdValue,
                                unordered_map<int, double>**pixelEnergyMapPtr,
                                unsigned int *eventsInFrame)
{
   return pixelProcessor->processFrame(frame, thresholdValue, pixelEnergyMapPtr,
                                       eventsInFrame);
}


double *FrameProcessor::process(uint16_t *frame, uint16_t *thresholdPerPixel,
                                unordered_map<int, double>**pixelEnergyMapPtr,
                                unsigned int *eventsInFrame)
{
   return pixelProcessor->processFrame(frame, thresholdPerPixel, pixelEnergyMapPtr,
                                       eventsInFrame);
}
