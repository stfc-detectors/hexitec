#ifndef HXTGENERATOR_H
#define HXTGENERATOR_H

#include "generalhxtgenerator.h"

class HxtGenerator : public GeneralHxtGenerator
{
public:
//   HxtGenerator();
   HxtGenerator(int frameSize, long long binStart, long long binEnd, double binWidth);
//   double *getNextPixelEnergy();
   void processEnergies(double *pixelEnergy);

public slots:
  void handleProcess();
};

#endif // HXTGENERATOR_H
