#ifndef GENERALFRAMEPROCESSOR_H
#define GENERALFRAMEPROCESSOR_H

#include "pixelProcessor.h"
#include <cstdint>

class GeneralFrameProcessor
{
public:
   GeneralFrameProcessor();
   ~GeneralFrameProcessor();
   void setGradients(double *gradientValue);
   void setIntercepts(double *interceptValue);
   void setEnergyCalibration(bool energyCalibration);
   PixelProcessor *getPixelProcessor();

   virtual uint16_t *process(uint16_t *frame){return NULL;}
   virtual uint16_t *process(uint16_t *frame, uint16_t thresholdValue){return NULL;}
   virtual uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel){return NULL;}

protected:
   PixelProcessor *pixelProcessor;
   double *getData(const char *filename);
};

#endif // GENERALFRAMEPROCESSOR_H
