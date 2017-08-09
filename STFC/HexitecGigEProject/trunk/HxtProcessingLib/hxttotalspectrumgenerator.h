#ifndef HXTTOTALSPECTRUMGENERATOR_H
#define HXTTOTALSPECTRUMGENERATOR_H

#include "generalhxtgenerator.h"

class HxtTotalSpectrumGenerator : public GeneralHxtGenerator
{
public:
//   HxtTotalSpectrumGenerator();
   HxtTotalSpectrumGenerator(int frameSize, long long binStart, long long binEnd, double binWidth);
   ~HxtTotalSpectrumGenerator();

public slots:
   void handleProcess();
};

#endif // HXTTOTALSPECTRUMGENERATOR_H
