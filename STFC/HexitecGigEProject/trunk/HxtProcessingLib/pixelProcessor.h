#ifndef PIXELPROCESSOR_H
#define PIXELPROCESSOR_H

#include <QObject>
#include <cstdint>
#include <string>
#include <unordered_map>
#include "generalpixelProcessor.h"

using namespace std;

class PixelProcessor : public GeneralPixelProcessor
{
   Q_OBJECT

public:
   PixelProcessor();
   /*
   uint16_t *processFrame(uint16_t *frame);
   uint16_t *processFrame(uint16_t *frame, uint16_t thresholdValue);
   uint16_t *processFrame(uint16_t *frame, uint16_t *thresholdPerPixel);
   uint16_t *processFrame(uint16_t *frame, double **pixelEnergyPtr);
   uint16_t *processFrame(uint16_t *frame, uint16_t thresholdValue, double **pixelEnergyPtr);
   uint16_t *processFrame(uint16_t *frame, uint16_t *thresholdPerPixel, double **pixelEnergyPtr);
   uint16_t *processRe_orderFrame(uint16_t *frame);
   uint16_t *processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue);
   uint16_t *processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel);
   uint16_t *processRe_orderFrame(uint16_t *frame, double **pixelEnergyPtr);
   uint16_t *processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue, double **pixelEnergyPtr);
   uint16_t *processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel, double **pixelEnergyPtr);


   uint16_t *processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
                                                  unordered_map<int, double>**pixelEnergyPtr);
   */

};

#endif // PIXELPROCESSOR_H
