#ifndef HXTGENERATOR_H
#define HXTGENERATOR_H

#include "generalhxtgenerator.h"

class HxtGenerator : public GeneralHxtGenerator
{
public:
   HxtGenerator();

public slots:
   void handleEnqueuePixelEnergy(double *pixelEnergy);
};

#endif // HXTGENERATOR_H
