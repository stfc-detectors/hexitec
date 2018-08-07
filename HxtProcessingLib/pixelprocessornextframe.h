#ifndef PIXELPROCESSORNEXTFRAME_H
#define PIXELPROCESSORNEXTFRAME_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include "generalpixelProcessor.h"

using namespace std;

class PixelProcessorNextFrame : public GeneralPixelProcessor
{

public:
   PixelProcessorNextFrame();

   void resetLastRe_orderedSize();

   double *processFrame(uint16_t *frame, uint16_t thresholdValue);
   double *processFrame(uint16_t *frame, uint16_t *thresholdPerPixel);

   double *processFrame(uint16_t *frame, uint16_t thresholdValue,
                                  unordered_map<int, double>**pixelEnergyMapPtr);
   double *processFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                  unordered_map<int, double>**pixelEnergyMapPtr);

  
   double *processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue);
   double *processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel);

   virtual double *processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
                                  unordered_map<int, double>**pixelEnergyMapPtr);
   double *processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                  unordered_map<int, double>**pixelEnergyMapPtr);

private:
   uint16_t  *lastRe_orderedFrame;

};


#endif // PIXELPROCESSORNEXTFRAME_H
