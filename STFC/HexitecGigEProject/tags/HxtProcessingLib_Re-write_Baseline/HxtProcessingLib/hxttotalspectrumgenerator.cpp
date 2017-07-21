#include "hxttotalspectrumgenerator.h"

#include <QDebug>

HxtTotalSpectrumGenerator::HxtTotalSpectrumGenerator()
{

}

HxtTotalSpectrumGenerator::~HxtTotalSpectrumGenerator()
{

}

void HxtTotalSpectrumGenerator::handleEnqueuePixelEnergy(double *pixelEnergy)
{
   qDebug() << "HxtTotalSpectrumGenerator::handleEnqueuePixelEnergy() called";
}
