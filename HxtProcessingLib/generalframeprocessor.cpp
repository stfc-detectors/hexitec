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

void GeneralFrameProcessor::setFrameInSize(uint32_t frameSize)
{
   pixelProcessor->setFrameInSize(frameSize);
   pixelProcessor->resetLastRe_orderedSize();
}

void GeneralFrameProcessor::setFrameInCols(uint32_t columns)
{
   pixelProcessor->setFrameInCols(columns);
}

void GeneralFrameProcessor::setFrameInRows(uint32_t rows)
{
    pixelProcessor->setFrameInRows(rows);
}

void GeneralFrameProcessor::setFrameOutSize(uint32_t frameSize)
{
   pixelProcessor->setFrameOutSize(frameSize);
   pixelProcessor->resetLastRe_orderedSize();
}

void GeneralFrameProcessor::setFrameOutCols(uint32_t columns)
{
   pixelProcessor->setFrameOutCols(columns);
}

void GeneralFrameProcessor::setFrameOutRows(uint32_t rows)
{
    pixelProcessor->setFrameOutRows(rows);
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

