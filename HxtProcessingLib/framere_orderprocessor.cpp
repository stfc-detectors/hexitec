#include "framere_orderprocessor.h"
//
#include <QDebug>
FrameRe_orderProcessor::FrameRe_orderProcessor(bool nextFrameCorrection) :
   GeneralFrameProcessor(nextFrameCorrection)
{
}

double *FrameRe_orderProcessor::process(unordered_map<int, double>**pixelRawValMapPtr,
                                          uint16_t *frame)
{
   double *result;

   result = pixelProcessor->processRe_orderFrame(pixelRawValMapPtr, frame);

   return result;
}

double *FrameRe_orderProcessor::process(unordered_map<int, double>**pixelRawValMapPtr,
                                          uint16_t *frame, uint16_t thresholdValue)
{
   double *result;

   result = pixelProcessor->processRe_orderFrame(pixelRawValMapPtr, frame, thresholdValue);

   return result;
}

double *FrameRe_orderProcessor::process(unordered_map<int, double>**pixelRawValMapPtr,
                                          uint16_t *frame, uint16_t *thresholdPerPixel)
{
   double *result;

   result = pixelProcessor->processRe_orderFrame(pixelRawValMapPtr, frame, thresholdPerPixel);

   return result;
}

double *FrameRe_orderProcessor::process(uint16_t *frame,
                                          unordered_map<int, double>**pixelEnergyMapPtr)
{
   double *result;
//   qDebug() << Q_FUNC_INFO;
   result = pixelProcessor->processRe_orderFrame(frame, pixelEnergyMapPtr);

   return result;
}

double *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t thresholdValue,
                                          unordered_map<int, double>**pixelEnergyMapPtr)
{
   double *result;

   result = pixelProcessor->processRe_orderFrame(frame, thresholdValue, pixelEnergyMapPtr);

   return result;
}

double *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t *thresholdPerPixel,
                                          unordered_map<int, double>**pixelEnergyMapPtr)
{
   double *result;

   result = pixelProcessor->processRe_orderFrame(frame, thresholdPerPixel, pixelEnergyMapPtr);

   return result;
}
