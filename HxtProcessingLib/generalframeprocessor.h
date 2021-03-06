#ifndef GENERALFRAMEPROCESSOR_H
#define GENERALFRAMEPROCESSOR_H

#include "pixelProcessor.h"
#include "pixelprocessornextframe.h"
#include <cstdint>
#include <unordered_map>

class GeneralFrameProcessor
{
public:
   GeneralFrameProcessor(bool nextFrameCorrection, int occupancyThreshold);
   ~GeneralFrameProcessor();
   void setFrameSize(int frameSize);
   void setGradients(double *gradientValue);
   void setIntercepts(double *interceptValue);
   void setEnergyCalibration(bool energyCalibration);
   GeneralPixelProcessor *getPixelProcessor();

   int getOccupancyCorrections() { return pixelProcessor->getOccupancyCorrections(); }

   virtual double *process(unordered_map<int, double>**pixelRawValMapPtr,
                           uint16_t *frame, unsigned int *eventsInFrame) = 0;
   virtual double *process(unordered_map<int, double>**pixelRawValMapPtr,
                           uint16_t *frame, uint16_t thresholdValue,
                           unsigned int *eventsInFrame) = 0;
   virtual double *process(unordered_map<int, double>**pixelRawValMapPtr,
                           uint16_t *frame, uint16_t *thresholdPerPixel,
                           unsigned int *eventsInFrame) = 0;

   virtual double *process(uint16_t *frame,
                           unordered_map<int, double>**pixelEnergyMapPtr,
                           unsigned int *eventsInFrame) = 0;
   virtual double *process(uint16_t *frame, uint16_t thresholdValue,
                           unordered_map<int, double>**pixelEnergyMapPtr,
                           unsigned int *eventsInFrame) = 0;
   virtual double *process(uint16_t *frame, uint16_t *thresholdPerPixel,
                           unordered_map<int, double>**pixelEnergyMapPtr,
                           unsigned int *eventsInFrame) = 0;

protected:
   GeneralPixelProcessor *pixelProcessor;
   double *getData(const char *filename);
   int occupancyThreshold;
};

#endif // GENERALFRAMEPROCESSOR_H
