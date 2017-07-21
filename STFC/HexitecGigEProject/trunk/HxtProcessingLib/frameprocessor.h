#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

#include "generalframeprocessor.h"

#include <string>
#include <cstdint>
#include <list>
#include <regex>

using namespace std;

class FrameProcessor : public GeneralFrameProcessor
{

public:
   FrameProcessor(GeneralHxtGenerator *hxtGenerator);
   ~FrameProcessor();
//   void setGradients(double *gradientValue);
//   void setIntercepts(double *interceptValue);
   uint16_t *process(uint16_t *frame);
   uint16_t *process(uint16_t *frame, uint16_t thresholdValue);
   uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel);
   PixelProcessor *getPixelProcessor();
   /*
   uint16_t *processRe_order(uint16_t *frame);
   uint16_t *processRe_order(uint16_t *frame, uint16_t thresholdValue);
   uint16_t *processRe_order(uint16_t *frame, uint16_t *thresholdPerPixel);
   uint16_t *process(uint16_t *frame, uint16_t thresholdValue, double *gradientValue, double *interceptValue);
   uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel, double *gradientValue, double *interceptValue);
*/
private:
//   GeneralHxtGenerator *hxtGenerator;
   void writeFile(uint16_t *result, const char *filename);
//   double *getData(const char *filename);
};

#endif // FRAMEPROCESSOR_H
