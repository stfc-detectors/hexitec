#ifndef GENERALFRAMEPROCESSOR_H
#define GENERALFRAMEPROCESSOR_H

#include "pixelProcessor.h"
#include "generalhxtgenerator.h"
#include <cstdint>

class GeneralFrameProcessor
{
public:
   GeneralFrameProcessor(GeneralHxtGenerator *hxtGenerator);
   ~GeneralFrameProcessor();
   void setGradients(double *gradientValue);
   void setIntercepts(double *interceptValue);
   void setEnergyCalibration(bool energyCalibration);
   PixelProcessor *getPixelProcessor();

   virtual uint16_t *process(uint16_t *frame) = 0;
   virtual uint16_t *process(uint16_t *frame, uint16_t thresholdValue) = 0;
   virtual uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel) = 0;

protected:
   GeneralHxtGenerator *hxtGenerator;
   PixelProcessor *pixelProcessor;
   uint16_t *processEnergy(uint16_t *frame, double **pixelEnergyPtr = NULL);
   double *getData(const char *filename);
};

#endif // GENERALFRAMEPROCESSOR_H
