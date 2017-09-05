#include "generalframeprocessor.h"

#include <iostream>
#include <fstream>
#include <QDebug>

GeneralFrameProcessor::GeneralFrameProcessor(bool nextFrameCorrection)
{
   qDebug() << "Calling GeneralFrameProcessor constructor nextFrameCorrection =" << nextFrameCorrection;
   if (nextFrameCorrection)
   {
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
   qDebug() << "pixelProcessor->setEnergyCalibration(energyCalibration): " << energyCalibration;
}

GeneralPixelProcessor *GeneralFrameProcessor::getPixelProcessor()
{
   return pixelProcessor;
}

