#include "framere_orderprocessor.h"

#include <QDebug>

FrameRe_orderProcessor::FrameRe_orderProcessor(bool nextFrameCorrection)
{
   nextFrameCorrection = true;
   if (nextFrameCorrection)
   {
      pixelProcessor = new PixelProcessorNextFrame();
   }
   else
   {
      pixelProcessor = new PixelProcessor();
   }
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//re_order.bin";

   result = pixelProcessor->processRe_orderFrame(frame);
//   writeFile(result, filename);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//re_orderThreshVal.bin";

   result = pixelProcessor->processRe_orderFrame(frame, thresholdValue);
//   writeFile(result, filename);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t *thresholdPerPix)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//re_orderThreshPerPix.bin";

   result = pixelProcessor->processRe_orderFrame(frame, thresholdPerPix);
//   writeFile(result, filename);

   return result;
}
uint16_t *FrameRe_orderProcessor::process(uint16_t *frame,
                                          unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//re_order.bin";

   result = pixelProcessor->processRe_orderFrame(frame, pixelEnergyMapPtr);
//   writeFile(result, filename);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t thresholdValue,
                                          unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//re_orderThreshVal.bin";

   result = pixelProcessor->processRe_orderFrame(frame, thresholdValue, pixelEnergyMapPtr);
//   writeFile(result, filename);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t *thresholdPerPix,
                                          unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//re_orderThreshPerPix.bin";

   result = pixelProcessor->processRe_orderFrame(frame, thresholdPerPix, pixelEnergyMapPtr);
//   writeFile(result, filename);

   return result;
}
