#ifndef HXTGENERATOR_H
#define HXTGENERATOR_H

#include "generalhxtgenerator.h"

class HxtGenerator : public GeneralHxtGenerator
{
public:
   HxtGenerator(int frameSize, unsigned long long binStart, unsigned long long binEnd, unsigned long long binWidth);
   double *getNextPixelEnergy();
   void processEnergies(double *pixelEnergy);

public slots:
  void handleProcess();
//  void enqueuePixelEnergy(double *pixelEnergy);
};

#endif // HXTGENERATOR_H
