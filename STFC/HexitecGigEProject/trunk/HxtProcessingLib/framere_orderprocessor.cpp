#include "framere_orderprocessor.h"

#include <QDebug>

FrameRe_orderProcessor::FrameRe_orderProcessor(bool nextFrameCorrection) :
   GeneralFrameProcessor(nextFrameCorrection)
{
   qDebug() << "Calling FrameRe_orderProcessor constructor";
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame)
{
   uint16_t *result;

   qDebug() << "process 11111";
   result = pixelProcessor->processRe_orderFrame(frame);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t *result;

   qDebug() << "process 22222";
   result = pixelProcessor->processRe_orderFrame(frame, thresholdValue);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t *thresholdPerPix)
{
   uint16_t *result;

   qDebug() << "process 33333";
   result = pixelProcessor->processRe_orderFrame(frame, thresholdPerPix);

   return result;
}
uint16_t *FrameRe_orderProcessor::process(uint16_t *frame,
                                          unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t *result;

   qDebug() << "process 44444";
   result = pixelProcessor->processRe_orderFrame(frame, pixelEnergyMapPtr);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t thresholdValue,
                                          unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t *result;

   qDebug() << "process 55555";
   result = pixelProcessor->processRe_orderFrame(frame, thresholdValue, pixelEnergyMapPtr);

   return result;
}

uint16_t *FrameRe_orderProcessor::process(uint16_t *frame, uint16_t *thresholdPerPix,
                                          unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t *result;

   qDebug() << "process 66666";
   result = pixelProcessor->processRe_orderFrame(frame, thresholdPerPix, pixelEnergyMapPtr);

   return result;
}
