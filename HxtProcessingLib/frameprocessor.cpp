#include "frameprocessor.h"
#include <iostream>
#include <fstream>
//
//#include <QTime>
#include <qDebug>

FrameProcessor::FrameProcessor(bool nextFrameCorrection) :
   GeneralFrameProcessor(nextFrameCorrection)
{
}

uint16_t *FrameProcessor::process(unordered_map<int, double> **pixelRawValMapPtr, uint16_t *frame)
{
   uint16_t *result;

   result = pixelProcessor->processFrame(pixelRawValMapPtr, frame);

   return result;
}

uint16_t *FrameProcessor::process(unordered_map<int, double>**pixelRawValMapPtr, uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t *result;

   result = pixelProcessor->processFrame(pixelRawValMapPtr, frame, thresholdValue);

   return result;
}

uint16_t *FrameProcessor::process(unordered_map<int, double>**pixelRawValMapPtr, uint16_t *frame, uint16_t *thresholdPerPix)
{
   uint16_t *result;

   result = pixelProcessor->processFrame(pixelRawValMapPtr, frame, thresholdPerPix);

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
//   QTime qtTime;   int resultTime = 0;   qtTime.restart();
   result = pixelProcessor->processFrame(frame, thresholdValue, pixelEnergyMapPtr);
//   resultTime = qtTime.elapsed();
//   qDebug() << "FP    result: " << (resultTime) << " ms.";

   return result;
}

uint16_t *FrameProcessor::process(uint16_t *frame, uint16_t *thresholdPerPix,
                                  unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t *result;

   result = pixelProcessor->processFrame(frame, thresholdPerPix, pixelEnergyMapPtr);

   return result;
}
