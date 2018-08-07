#include "frameprocessor.h"
#include <iostream>
#include <fstream>

FrameProcessor::FrameProcessor(bool nextFrameCorrection) :
   GeneralFrameProcessor(nextFrameCorrection)
{
}


double *FrameProcessor::process(unordered_map<int, double> **pixelRawValMapPtr, uint16_t *frame)
{
   return pixelProcessor->processFrame(pixelRawValMapPtr, frame);
}


double *FrameProcessor::process(unordered_map<int, double>**pixelRawValMapPtr, uint16_t *frame, uint16_t thresholdValue)
{
   return pixelProcessor->processFrame(pixelRawValMapPtr, frame, thresholdValue);
}


double *FrameProcessor::process(unordered_map<int, double>**pixelRawValMapPtr, uint16_t *frame, uint16_t *thresholdPerPixel)
{
   return pixelProcessor->processFrame(pixelRawValMapPtr, frame, thresholdPerPixel);
}


double *FrameProcessor::process(uint16_t *frame,
                                  unordered_map<int, double>**pixelEnergyMapPtr)
{
   return pixelProcessor->processFrame(frame, pixelEnergyMapPtr);
}


double *FrameProcessor::process(uint16_t *frame, uint16_t thresholdValue,
                                  unordered_map<int, double>**pixelEnergyMapPtr)
{
   return pixelProcessor->processFrame(frame, thresholdValue, pixelEnergyMapPtr);
}


double *FrameProcessor::process(uint16_t *frame, uint16_t *thresholdPerPixel,
                                  unordered_map<int, double>**pixelEnergyMapPtr)
{
   return pixelProcessor->processFrame(frame, thresholdPerPixel, pixelEnergyMapPtr);
}
