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
   PixelProcessorNextFrame(int occupancyThreshold);

   void resetLastRe_orderedSize();
   int getOccupancyCorrections() { return occupancyCorrections; }

   double *processFrame(uint16_t *frame, uint16_t thresholdValue,
                        unsigned int *eventsInFrame);
   double *processFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                        unsigned int *eventsInFrame);

   double *processFrame(uint16_t *frame, uint16_t thresholdValue,
                        unordered_map<int, double>**pixelEnergyMapPtr,
                        unsigned int *eventsInFrame);
   double *processFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                        unordered_map<int, double>**pixelEnergyMapPtr,
                        unsigned int *eventsInFrame);

  
   double *processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
                                unsigned int *eventsInFrame);
   double *processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                unsigned int *eventsInFrame);

   virtual double *processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
                                        unordered_map<int, double>**pixelEnergyMapPtr,
                                        unsigned int *eventsInFrame);
   double *processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                unordered_map<int, double>**pixelEnergyMapPtr,
                                unsigned int *eventsInFrame);

private:
   uint16_t  *lastRe_orderedFrame;
   ///
   void writeFile(const char *buffer, unsigned long length, std::string filePrefix);
};


#endif // PIXELPROCESSORNEXTFRAME_H
