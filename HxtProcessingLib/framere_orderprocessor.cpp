#include "framere_orderprocessor.h"

FrameRe_orderProcessor::FrameRe_orderProcessor(bool nextFrameCorrection, int occupancyThreshold ) :
   GeneralFrameProcessor(nextFrameCorrection, occupancyThreshold)
{
}


double *FrameRe_orderProcessor::process(unordered_map<int, double>**pixelRawValMapPtr,
                                          uint16_t *frame)
{
   return pixelProcessor->processRe_orderFrame(pixelRawValMapPtr, frame);
}


double *FrameRe_orderProcessor::process(unordered_map<int, double>**pixelRawValMapPtr,
                                          uint16_t *frame, uint16_t thresholdValue)
{
   return pixelProcessor->processRe_orderFrame(pixelRawValMapPtr, frame, thresholdValue);
}


double *FrameRe_orderProcessor::process(unordered_map<int, double>**pixelRawValMapPtr,
                                          uint16_t *frame, uint16_t *thresholdPerPixel)
{
   return pixelProcessor->processRe_orderFrame(pixelRawValMapPtr, frame, thresholdPerPixel);
}


double *FrameRe_orderProcessor::process(uint16_t *frame,
                                          unordered_map<int, double>**pixelEnergyMapPtr)
{
   return pixelProcessor->processRe_orderFrame(frame, pixelEnergyMapPtr);
}


double *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t thresholdValue,
                                          unordered_map<int, double>**pixelEnergyMapPtr)
{
   return pixelProcessor->processRe_orderFrame(frame, thresholdValue, pixelEnergyMapPtr);
}


double *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t *thresholdPerPixel,
                                          unordered_map<int, double>**pixelEnergyMapPtr)
{
   return pixelProcessor->processRe_orderFrame(frame, thresholdPerPixel, pixelEnergyMapPtr);
}
