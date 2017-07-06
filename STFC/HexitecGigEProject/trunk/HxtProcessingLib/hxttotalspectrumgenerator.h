#ifndef HXTTOTALSPECTRUMGENERATOR_H
#define HXTTOTALSPECTRUMGENERATOR_H

#include "generalhxtgenerator.h"

class HxtTotalSpectrumGenerator : public GeneralHxtGenerator
{
public:
   HxtTotalSpectrumGenerator();
   ~HxtTotalSpectrumGenerator();

public slots:
   void handleEnqueuePixelEnergy(double *pixelEnergy);
};

#endif // HXTTOTALSPECTRUMGENERATOR_H
