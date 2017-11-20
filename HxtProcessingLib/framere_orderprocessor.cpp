#include "framere_orderprocessor.h"

FrameRe_orderProcessor::FrameRe_orderProcessor(bool nextFrameCorrection) :
   GeneralFrameProcessor(nextFrameCorrection)
{
}

uint16_t *FrameRe_orderProcessor::process(unordered_map<int, double>**pixelRawValMapPtr, 
                                          uint16_t *frame)
{
   uint16_t *result;

   result = pixelProcessor->processRe_orderFrame(pixelRawValMapPtr, frame);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(unordered_map<int, double>**pixelRawValMapPtr, 
                                          uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t *result;

   result = pixelProcessor->processRe_orderFrame(pixelRawValMapPtr, frame, thresholdValue);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(unordered_map<int, double>**pixelRawValMapPtr, 
                                          uint16_t *frame, uint16_t *thresholdPerPix)
{
   uint16_t *result;

   result = pixelProcessor->processRe_orderFrame(pixelRawValMapPtr, frame, thresholdPerPix);

   return result;
}
uint16_t *FrameRe_orderProcessor::process(uint16_t *frame,
                                          unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t *result;

   result = pixelProcessor->processRe_orderFrame(frame, pixelEnergyMapPtr);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t thresholdValue,
                                          unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t *result;

   result = pixelProcessor->processRe_orderFrame(frame, thresholdValue, pixelEnergyMapPtr);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t *thresholdPerPix,
                                          unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t *result;

   result = pixelProcessor->processRe_orderFrame(frame, thresholdPerPix, pixelEnergyMapPtr);

   return result;
}
