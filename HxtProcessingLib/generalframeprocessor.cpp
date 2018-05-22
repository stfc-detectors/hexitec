#include "generalframeprocessor.h"

#include <iostream>
#include <fstream>
//
#include <QDebug>
GeneralFrameProcessor::GeneralFrameProcessor(bool nextFrameCorrection)
{
   if (nextFrameCorrection)
   {
      qDebug() << Q_FUNC_INFO << " CTOR";
      pixelProcessor = new PixelProcessorNextFrame();
   }
   else
   {
      pixelProcessor = new PixelProcessor();
   }
}

GeneralFrameProcessor::~GeneralFrameProcessor()
{
   delete pixelProcessor;
}

void GeneralFrameProcessor::setFrameSize(int frameSize)
{
   pixelProcessor->setFrameSize(frameSize);
}


void GeneralFrameProcessor::setGradients(double *gradientValue)
{
   pixelProcessor->setGradientValue(gradientValue);
}

void GeneralFrameProcessor::setIntercepts(double *interceptValue)
{
   pixelProcessor->setInterceptValue(interceptValue);
}

void GeneralFrameProcessor::setEnergyCalibration(bool energyCalibration)
{
   pixelProcessor->setEnergyCalibration(energyCalibration);
}

GeneralPixelProcessor *GeneralFrameProcessor::getPixelProcessor()
{
   return pixelProcessor;
}

