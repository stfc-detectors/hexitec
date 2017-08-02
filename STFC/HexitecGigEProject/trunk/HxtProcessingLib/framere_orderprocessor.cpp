#include "framere_orderprocessor.h"

#include <QDebug>

FrameRe_orderProcessor::FrameRe_orderProcessor()
{
   pixelProcessor = new PixelProcessor();
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_order.bin";

   result = pixelProcessor->processRe_orderFrame(frame);
//   writeFile(result, filename);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshVal.bin";

   result = pixelProcessor->processRe_orderFrame(frame, thresholdValue);
//   writeFile(result, filename);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t *thresholdPerPix)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshPerPix.bin";

   result = pixelProcessor->processRe_orderFrame(frame, thresholdPerPix);
//   writeFile(result, filename);

   return result;
}
uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, double **pixelEnergyPtr)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_order.bin";

   result = pixelProcessor->processRe_orderFrame(frame, pixelEnergyPtr);
//   writeFile(result, filename);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t thresholdValue, double **pixelEnergyPtr)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshVal.bin";

   result = pixelProcessor->processRe_orderFrame(frame, thresholdValue, pixelEnergyPtr);
//   writeFile(result, filename);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t *thresholdPerPix, double **pixelEnergyPtr)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshPerPix.bin";

   result = pixelProcessor->processRe_orderFrame(frame, thresholdPerPix, pixelEnergyPtr);
//   writeFile(result, filename);

   return result;
}
