#ifndef FRAMERE_ORDERPROCESSOR_H
#define FRAMERE_ORDERPROCESSOR_H

#include "generalframeprocessor.h"

using namespace std;

class FrameRe_orderProcessor : public GeneralFrameProcessor
{
public:
   FrameRe_orderProcessor(GeneralHxtGenerator *hxtGenerator);
   ~FrameRe_orderProcessor();
   uint16_t *process(uint16_t *frame);
   uint16_t *process(uint16_t *frame, uint16_t thresholdValue);
   uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel);
};

#endif // FRAMERE_ORDERPROCESSOR_H
