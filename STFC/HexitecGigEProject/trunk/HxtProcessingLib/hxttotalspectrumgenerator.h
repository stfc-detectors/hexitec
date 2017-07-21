#ifndef HXTTOTALSPECTRUMGENERATOR_H
#define HXTTOTALSPECTRUMGENERATOR_H

#include "generalhxtgenerator.h"

class HxtTotalSpectrumGenerator : public GeneralHxtGenerator
{
public:
   HxtTotalSpectrumGenerator(int frameSize, unsigned long long binStart, unsigned long long binEnd, unsigned long long binWidth);
   ~HxtTotalSpectrumGenerator();
   void processEnergies(double *pixelEnergy);

private:
   HxtItem *totalSpectrumItem;

public slots:
   void handleProcess();
//   void enqueuePixelEnergy(double *pixelEnergy);
};

#endif // HXTTOTALSPECTRUMGENERATOR_H
