#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

#include "pixelProcessor.h"

#include <string>
#include <cstdint>
#include <list>
#include <regex>

using namespace std;

class FrameProcessor
{

public:
   FrameProcessor();
   ~FrameProcessor();
   void setGradients(const   char* filename);
   void setIntercepts(const   char* filename);
   uint16_t *process(uint16_t *frame);
   uint16_t *process(uint16_t *frame, uint16_t thresholdValue);
   uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel);
   uint16_t *process(uint16_t *frame, uint16_t thresholdValue, double *gradientValue, double *interceptValue);
   uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel, double *gradientValue, double *interceptValue);

private:
   PixelProcessor *pixelProcessor;
   void writeFile(uint16_t *result, const char *filename);
   double *getData(const char *filename);
};

#endif // FRAMEPROCESSOR_H
