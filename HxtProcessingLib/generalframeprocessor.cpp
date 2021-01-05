#include "generalframeprocessor.h"

#include <iostream>
#include <fstream>

GeneralFrameProcessor::GeneralFrameProcessor(bool nextFrameCorrection, int occupancyThreshold)
{
   if (nextFrameCorrection)
   {
      pixelProcessor = new PixelProcessorNextFrame(occupancyThreshold);
   }
   else
   {
      pixelProcessor = new PixelProcessor(occupancyThreshold);
   }
   this->occupancyThreshold = occupancyThreshold;
}

GeneralFrameProcessor::~GeneralFrameProcessor()
{
   delete pixelProcessor;
}

void GeneralFrameProcessor::setFrameSize(int frameSize)
{
   pixelProcessor->setFrameSize(frameSize);
   pixelProcessor->resetLastRe_orderedSize();
}

void GeneralFrameProcessor::setCols(int columns)
{
   pixelProcessor->setCols(columns);
}

void GeneralFrameProcessor::setRows(int rows)
{
    pixelProcessor->setRows(rows);
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

