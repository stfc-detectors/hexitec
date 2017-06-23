#ifndef PIXELPROCESSOR_H
#define PIXELPROCESSOR_H

#include <cstdint>
#include <string>

using namespace std;

class PixelProcessor
{
public:
   PixelProcessor();
   void initialiseEnergyCalibration(double *gradientValue, double *interceptValue);
   double *getGradientValue();
   double *getInterceptValue();
   uint16_t *re_orderFrame(uint16_t *frame,
                           double *pixelEnergy = NULL);
   uint16_t *re_orderFrame(uint16_t *frame, uint16_t thresholdValue,
                           double *pixelEnergy = NULL);
   uint16_t *re_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                           double *pixelEnergy = NULL);

private:
   void initialisePixelMap();
   uint16_t pixelMap[6400];
   uint16_t frameSize;
   double *gradientValue;
   double *interceptValue;
};

#endif // PIXELPROCESSOR_H
