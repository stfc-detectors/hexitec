#include "frameprocessor.h"
#include <iostream>
#include <fstream>
#include <QDebug>

FrameProcessor::FrameProcessor(bool nextFrameCorrection) :
   GeneralFrameProcessor(nextFrameCorrection)
{
   qDebug() << "Calling FrameProcessor constructor";
}

uint16_t *FrameProcessor::process(uint16_t *frame)
{
   uint16_t *result;

   result = pixelProcessor->processFrame(frame);

   return result;
}

uint16_t *FrameProcessor::process(uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t *result;

   result = pixelProcessor->processFrame(frame, thresholdValue);

   return result;
}

uint16_t *FrameProcessor::process(uint16_t *frame, uint16_t *thresholdPerPix)
{
   uint16_t *result;

   result = pixelProcessor->processFrame(frame, thresholdPerPix);

   return result;
}

uint16_t *FrameProcessor::process(uint16_t *frame,
                                  unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t *result;

   result = pixelProcessor->processFrame(frame, pixelEnergyMapPtr);

   return result;
}

uint16_t *FrameProcessor::process(uint16_t *frame, uint16_t thresholdValue,
                                  unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t *result;

   result = pixelProcessor->processFrame(frame, thresholdValue, pixelEnergyMapPtr);

   return result;
}

uint16_t *FrameProcessor::process(uint16_t *frame, uint16_t *thresholdPerPix,
                                  unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t *result;

   result = pixelProcessor->processFrame(frame, thresholdPerPix, pixelEnergyMapPtr);

   return result;
}
