#include "generalframeprocessor.h"

#include <iostream>
#include <fstream>
#include <QDebug>

GeneralFrameProcessor::GeneralFrameProcessor()
{

}

GeneralFrameProcessor::~GeneralFrameProcessor()
{
   free(pixelProcessor);
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

PixelProcessor *GeneralFrameProcessor::getPixelProcessor()
{
   return pixelProcessor;
}

double *GeneralFrameProcessor::getData(const char *filename)
{
   int i = 0;
   double *dataValue;
   std::ifstream inFile;

   dataValue = (double *) calloc(6400, sizeof(double));
   inFile.open(filename);

   if (!inFile)
     qDebug() << "error opening " << filename;
   while (inFile >> dataValue[i])
   {
       i++;
   }

   if (i < 6400)
     qDebug() << "error: only " << i << " could be read";
   else
     qDebug() << "gradients read OK ";
   inFile.close();

   return dataValue;
}
