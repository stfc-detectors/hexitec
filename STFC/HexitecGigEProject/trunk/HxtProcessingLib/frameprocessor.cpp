#include "frameprocessor.h"
#include <iostream>
#include <fstream>
#include <QDebug>

FrameProcessor::FrameProcessor()
{
   pixelProcessor = new PixelProcessor();
}

FrameProcessor::~FrameProcessor()
{
   //   free((void *)re_ordered);
}

void FrameProcessor::setGradients(const char *filename)
{
   double *gradientValue;

   gradientValue = getData(filename);
   pixelProcessor->initialiseEnergyCalibration(gradientValue, pixelProcessor->getInterceptValue());
}

void FrameProcessor::setIntercepts(const char *filename)
{
   double *interceptValue;

   interceptValue = getData(filename);
   pixelProcessor->initialiseEnergyCalibration(pixelProcessor->getGradientValue(), interceptValue);
}

double *FrameProcessor::getData(const char *filename)
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

uint16_t *FrameProcessor::process(uint16_t *frame)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_order.bin";

   result = pixelProcessor->re_orderFrame(frame);
   writeFile(result, filename);

   return result;
}

uint16_t *FrameProcessor::process(uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshVal.bin";

   result = pixelProcessor->re_orderFrame(frame, thresholdValue);
   writeFile(result, filename);

   return result;
}

uint16_t *FrameProcessor::process(uint16_t *frame, uint16_t *thresholdPerPix)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshPerPix.bin";

   result = pixelProcessor->re_orderFrame(frame, thresholdPerPix, (double *)NULL);
   writeFile(result, filename);

   return result;
}

void FrameProcessor::writeFile(uint16_t *result, const char* filename)
{
   std::ofstream outFile;

   outFile.open(filename, std::ofstream::binary | std::ofstream::app);
   outFile.write((const char *)result, 6400 * sizeof(uint16_t));
   outFile.close();

}
