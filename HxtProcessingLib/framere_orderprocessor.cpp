#include "framere_orderprocessor.h"

FrameRe_orderProcessor::FrameRe_orderProcessor(bool nextFrameCorrection, int occupancyThreshold ) :
   GeneralFrameProcessor(nextFrameCorrection, occupancyThreshold)
{
}


double *FrameRe_orderProcessor::process(unordered_map<int, double>**pixelRawValMapPtr,
                                        uint16_t *frame, unsigned int *eventsInFrame)
{
   return pixelProcessor->processRe_orderFrame(pixelRawValMapPtr, frame, eventsInFrame);
}


double *FrameRe_orderProcessor::process(unordered_map<int, double>**pixelRawValMapPtr,
                                        uint16_t *frame, uint16_t thresholdValue,
                                        unsigned int *eventsInFrame)
{
   return pixelProcessor->processRe_orderFrame(pixelRawValMapPtr, frame, thresholdValue,
                                               eventsInFrame);
}


double *FrameRe_orderProcessor::process(unordered_map<int, double>**pixelRawValMapPtr,
                                        uint16_t *frame, uint16_t *thresholdPerPixel,
                                        unsigned int *eventsInFrame)
{
   return pixelProcessor->processRe_orderFrame(pixelRawValMapPtr, frame, thresholdPerPixel,
                                               eventsInFrame);
}


double *FrameRe_orderProcessor::process(uint16_t *frame,
                                        unordered_map<int, double>**pixelEnergyMapPtr,
                                        unsigned int *eventsInFrame)
{
   return pixelProcessor->processRe_orderFrame(frame, pixelEnergyMapPtr, eventsInFrame);
}


double *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t thresholdValue,
                                        unordered_map<int, double>**pixelEnergyMapPtr,
                                        unsigned int *eventsInFrame)
{
   return pixelProcessor->processRe_orderFrame(frame, thresholdValue, pixelEnergyMapPtr,
                                               eventsInFrame);
}


double *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t *thresholdPerPixel,
                                        unordered_map<int, double>**pixelEnergyMapPtr,
                                        unsigned int *eventsInFrame)
{
   return pixelProcessor->processRe_orderFrame(frame, thresholdPerPixel, pixelEnergyMapPtr,
                                               eventsInFrame);
}
