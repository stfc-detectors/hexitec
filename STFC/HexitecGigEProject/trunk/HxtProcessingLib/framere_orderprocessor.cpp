#include "framere_orderprocessor.h"

FrameRe_orderProcessor::FrameRe_orderProcessor(GeneralHxtGenerator *hxtGenerator) : GeneralFrameProcessor(hxtGenerator)
{
   this->hxtGenerator = hxtGenerator;
   pixelProcessor = new PixelProcessor();
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame)
{
   uint16_t *result;
   double **pixelEnergyPtr;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_order.bin";

   qDebug()<< "FrameRe_orderProcessor::process CALLED";
   result = pixelProcessor->processRe_orderFrame(frame, pixelEnergyPtr);
   processEnergy(frame, pixelEnergyPtr);
//   writeFile(result, filename);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t *result;
   double **pixelEnergyPtr;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshVal.bin";

   result = pixelProcessor->processRe_orderFrame(frame, thresholdValue, pixelEnergyPtr);
   processEnergy(frame, pixelEnergyPtr);
//   writeFile(result, filename);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t *thresholdPerPix)
{
   uint16_t *result;
   double **pixelEnergyPtr;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshPerPix.bin";

   result = pixelProcessor->processRe_orderFrame(frame, thresholdPerPix, pixelEnergyPtr);
   processEnergy(frame, pixelEnergyPtr);
//   writeFile(result, filename);

   return result;
}
