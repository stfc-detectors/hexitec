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

double *FrameProcessor::process(unordered_map<int, double> **pixelRawValMapPtr, uint16_t *frame)
{
   double *result;

   result = pixelProcessor->processFrame(pixelRawValMapPtr, frame);

   return result;
}

double *FrameProcessor::process(unordered_map<int, double>**pixelRawValMapPtr, uint16_t *frame, uint16_t thresholdValue)
{
   double *result;

   result = pixelProcessor->processFrame(pixelRawValMapPtr, frame, thresholdValue);

   return result;
}

double *FrameProcessor::process(unordered_map<int, double>**pixelRawValMapPtr, uint16_t *frame, uint16_t *thresholdPerPixel)
{
   double *result;

   result = pixelProcessor->processFrame(pixelRawValMapPtr, frame, thresholdPerPixel);

   return result;
}

double *FrameProcessor::process(uint16_t *frame,
                                  unordered_map<int, double>**pixelEnergyMapPtr)
{
   double *result;

   result = pixelProcessor->processFrame(frame, pixelEnergyMapPtr);

   return result;
}

double *FrameProcessor::process(uint16_t *frame, uint16_t thresholdValue,
                                  unordered_map<int, double>**pixelEnergyMapPtr)
{
//   qDebug() << Q_FUNC_INFO;
   double *result;
//   QTime qtTime;   int resultTime = 0;   qtTime.restart();
   result = pixelProcessor->processFrame(frame, thresholdValue, pixelEnergyMapPtr);
//   resultTime = qtTime.elapsed();
//   qDebug() << "FP    result: " << (resultTime) << " ms.";

   return result;
}

double *FrameProcessor::process(uint16_t *frame, uint16_t *thresholdPerPixel,
                                  unordered_map<int, double>**pixelEnergyMapPtr)
{
   double *result;

   result = pixelProcessor->processFrame(frame, thresholdPerPixel, pixelEnergyMapPtr);

   return result;
}
