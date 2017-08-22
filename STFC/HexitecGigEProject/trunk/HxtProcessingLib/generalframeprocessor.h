#ifndef GENERALFRAMEPROCESSOR_H
#define GENERALFRAMEPROCESSOR_H

#include "pixelProcessor.h"
#include <cstdint>
#include <unordered_map>

class GeneralFrameProcessor
{
public:
   GeneralFrameProcessor();
   ~GeneralFrameProcessor();
   void setGradients(double *gradientValue);
   void setIntercepts(double *interceptValue);
   void setEnergyCalibration(bool energyCalibration);
   GeneralPixelProcessor *getPixelProcessor();

   virtual uint16_t *process(uint16_t *frame) = 0;
   virtual uint16_t *process(uint16_t *frame, uint16_t thresholdValue) = 0;
   virtual uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel) = 0;

   virtual uint16_t *process(uint16_t *frame,
                             unordered_map<int, double>**pixelEnergyMapPtr) = 0;
   virtual uint16_t *process(uint16_t *frame, uint16_t thresholdValue,
                             unordered_map<int, double>**pixelEnergyMapPtr) = 0;
   virtual uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel,
                             unordered_map<int, double>**pixelEnergyMapPtr) = 0;

protected:
   GeneralPixelProcessor *pixelProcessor;
   double *getData(const char *filename);
};

#endif // GENERALFRAMEPROCESSOR_H
