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

   uint16_t *processFrame(uint16_t *frame, uint16_t thresholdValue);
   uint16_t *processFrame(uint16_t *frame, uint16_t *thresholdPerPixel);

   uint16_t *processFrame(uint16_t *frame, uint16_t thresholdValue,
                                  unordered_map<int, double>**pixelEnergyMapPtr);
   uint16_t *processFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                  unordered_map<int, double>**pixelEnergyMapPtr);

  
   uint16_t *processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue);
   uint16_t *processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel);

   virtual uint16_t *processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
                                  unordered_map<int, double>**pixelEnergyMapPtr);
   uint16_t *processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                  unordered_map<int, double>**pixelEnergyMapPtr);

private:
   uint16_t  *lastRe_orderedFrame;

};


#endif // PIXELPROCESSORNEXTFRAME_H
