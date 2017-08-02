#include "frameprocessor.h"
#include <iostream>
#include <fstream>
#include <QDebug>

FrameProcessor::FrameProcessor()
{
   pixelProcessor = new PixelProcessor();
}

uint16_t *FrameProcessor::process(uint16_t *frame)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_order.bin";

   result = pixelProcessor->processFrame(frame);
//   writeFile(result, filename);

   return result;
}

uint16_t *FrameProcessor::process(uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshVal.bin";

   result = pixelProcessor->processFrame(frame, thresholdValue);
//   writeFile(result, filename);

   return result;
}

uint16_t *FrameProcessor::process(uint16_t *frame, uint16_t *thresholdPerPix)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshPerPix.bin";

   result = pixelProcessor->processFrame(frame, thresholdPerPix);
//   writeFile(result, filename);

   return result;
}

uint16_t *FrameProcessor::process(uint16_t *frame, double **pixelEnergyPtr)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_order.bin";

   result = pixelProcessor->processFrame(frame, pixelEnergyPtr);
//   writeFile(result, filename);

   return result;
}

uint16_t *FrameProcessor::process(uint16_t *frame, uint16_t thresholdValue, double **pixelEnergyPtr)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshVal.bin";

   result = pixelProcessor->processFrame(frame, thresholdValue, pixelEnergyPtr);
//   writeFile(result, filename);

   return result;
}

uint16_t *FrameProcessor::process(uint16_t *frame, uint16_t *thresholdPerPix, double **pixelEnergyPtr)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshPerPix.bin";

   result = pixelProcessor->processFrame(frame, thresholdPerPix, pixelEnergyPtr);
//   writeFile(result, filename);

   return result;
}
/*
PixelProcessor *FrameProcessor::getPixelProcessor()
{
   return pixelProcessor;
}

void FrameProcessor::writeFile(uint16_t *result, const char* filename)
{
   std::ofstream outFile;

   outFile.open(filename, std::ofstream::binary | std::ofstream::app);
   outFile.write((const char *)result, 6400 * sizeof(uint16_t));
   outFile.close();

}
*/
