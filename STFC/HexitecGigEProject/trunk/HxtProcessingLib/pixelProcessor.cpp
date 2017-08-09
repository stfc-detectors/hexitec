#include "pixelProcessor.h"
#include <QDebug>

#include <iostream>
#include <fstream>
#include <intrin.h>
#include <bitset>
#include <vector>
#include <sys/stat.h>

static uint16_t pixelMap[6400];
static bool pixelMapInitialised = false;
static uint16_t frameSize = 80 * 80;


PixelProcessor::PixelProcessor()
{
   if (!pixelMapInitialised)
   {
      initialisePixelMap();
      pixelMapInitialised = true;
   }
   gradientValue = NULL;
   interceptValue = NULL;
}

void PixelProcessor::initialisePixelMap()
{
   int pmIndex = 0;

   for (int row = 0; row < 80; row++)
   {
      for (int col = 0; col < 20; col++)
      {
         for (int pix = 0; pix < 80; pix+=20)
         {
            pixelMap[pmIndex] = pix + col +(row * 80);
//            qDebug() << "pixamp " << pmIndex << " = " << pixelMap[pmIndex];
            pmIndex++;
         }
      }
   }
}

void PixelProcessor::setEnergyCalibration(bool energyCalibration)
{
   qDebug() << "PixelProcessor::setEnergyCalibration(): " << energyCalibration;
   this->energyCalibration = energyCalibration;
}

void PixelProcessor::setGradientValue(double *gradientValue)
{
   this->gradientValue = gradientValue;
}

void PixelProcessor::setInterceptValue(double *interceptValue)
{
   this->interceptValue = interceptValue;
}

void PixelProcessor::initialiseEnergyCalibration(double *gradientValue, double *interceptValue)
{
   this->gradientValue = gradientValue;
   this->interceptValue = interceptValue;
}

double *PixelProcessor::getGradientValue()
{
   return gradientValue;
}

double *PixelProcessor::getInterceptValue()
{
   return interceptValue;
}

uint16_t *PixelProcessor::processFrame(uint16_t *frame)
{
   uint16_t  *re_orderedFrame;
   uint16_t  *re_orderedFrameIndex;

   qDebug() << "PixelProcessor::processFrame() 1";
   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));
   re_orderedFrameIndex = re_orderedFrame;

   memcpy(re_orderedFrame, frame, frameSize * sizeof(uint16_t));

   return re_orderedFrame;
}

uint16_t *PixelProcessor::processFrame(uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t  *re_orderedFrame;
   double *pixelEnergy;

   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   qDebug() << "PixelProcessor::processFrame() 2";
   memcpy(re_orderedFrame, frame, frameSize * sizeof(uint16_t));
   for (int i = 0; i < frameSize; i++)
   {
      if (re_orderedFrame[i] - thresholdValue < 0)
      {
         re_orderedFrame[i] = 0;
      }
   }

   return re_orderedFrame;
}

uint16_t *PixelProcessor::processFrame(uint16_t *frame, uint16_t *thresholdPerPixel)
{
   uint16_t  *re_orderedFrame;
   double *pixelEnergy;

   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   qDebug() << "PixelProcessor::processFrame() 3";
   memcpy(re_orderedFrame, frame, frameSize * sizeof(uint16_t));
   for (int i = 0; i < frameSize; i++)
   {
      if (re_orderedFrame[i] - thresholdPerPixel[i] < 0)
      {
         re_orderedFrame[i] = 0;
      }
   }

   return re_orderedFrame;
}

uint16_t *PixelProcessor::processFrame(uint16_t *frame, double **pixelEnergyPtr)
{
   uint16_t  *re_orderedFrame;
   double *pixelEnergy;

   qDebug() << "PixelProcessor::processFrame()";
   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   pixelEnergy = (double *) calloc(frameSize, sizeof(double));
   memcpy(re_orderedFrame, frame, frameSize * sizeof(uint16_t));
   for (int i = 0; i < frameSize; i++)
   {
      pixelEnergy[i] = re_orderedFrame[i] * gradientValue[i] + interceptValue[i];
   }
   *pixelEnergyPtr = pixelEnergy;

   return re_orderedFrame;
}

uint16_t *PixelProcessor::processFrame(uint16_t *frame, uint16_t thresholdValue, double **pixelEnergyPtr)
{
   uint16_t  *re_orderedFrame;
   double *pixelEnergy;

   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   pixelEnergy = (double *) calloc(frameSize, sizeof(double));
   memcpy(re_orderedFrame, frame, frameSize * sizeof(uint16_t));
   for (int i = 0; i < frameSize; i++)
   {
      if (re_orderedFrame[i] - thresholdValue < 0)
      {
         re_orderedFrame[i] = 0;
         pixelEnergy[i] = interceptValue[i];
      }
      else
      {
         pixelEnergy[i] = re_orderedFrame[i] * gradientValue[i] + interceptValue[i];
      }
   }
   *pixelEnergyPtr = pixelEnergy;

   return re_orderedFrame;
}

uint16_t *PixelProcessor::processFrame(uint16_t *frame, uint16_t *thresholdPerPixel, double **pixelEnergyPtr)
{
   uint16_t  *re_orderedFrame;
   double *pixelEnergy;

   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   pixelEnergy = (double *) calloc(frameSize, sizeof(double));
   memcpy(re_orderedFrame, frame, frameSize * sizeof(uint16_t));
   for (int i = 0; i < frameSize; i++)
   {
      if (re_orderedFrame[i] - thresholdPerPixel[i] < 0)
      {
         re_orderedFrame[i] = 0;
         pixelEnergy[i] = interceptValue[i];
      }
      else
      {
         pixelEnergy[i] = re_orderedFrame[i] * gradientValue[i] + interceptValue[i];
      }
   }
   *pixelEnergyPtr = pixelEnergy;

   return re_orderedFrame;
}

uint16_t *PixelProcessor::processRe_orderFrame(uint16_t *frame)
{
   uint16_t  *re_orderedFrame;
   uint16_t  *re_orderedFrameIndex;

//   qDebug() << "PixelProcessor::processRe_orderFrame()";
   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));
   re_orderedFrameIndex = re_orderedFrame;

   for (int i = 0; i < frameSize; i++)
   {
      if (pixelMap[i] >= 6400 || pixelMap[i] < 0)
      {
         qDebug() << "pixelMap[i] has invalid value =" << pixelMap[i];
      }
      re_orderedFrame[pixelMap[i]] = frame[i];
   }

   return re_orderedFrame;
}

uint16_t *PixelProcessor::processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t  *re_orderedFrame;

   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   for (int i = 0; i < frameSize; i++)
   {
      if (frame[pixelMap[i]] - thresholdValue < 0)
      {
         re_orderedFrame[pixelMap[i]] =0;
      }
      else
      {
         re_orderedFrame[pixelMap[i]] = frame[i];
      }
   }

   return re_orderedFrame;
}

uint16_t *PixelProcessor::processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel)
{
   uint16_t  *re_orderedFrame;

   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   for (int i = 0; i < frameSize; i++)
   {
      if (frame[i] - thresholdPerPixel[i] < 0)
      {
         re_orderedFrame[pixelMap[i]] = 0;
      }
      else
      {
         re_orderedFrame[pixelMap[i]] = frame[i];
      }
   }

   return re_orderedFrame;
}

uint16_t *PixelProcessor::processRe_orderFrame(uint16_t *frame, double **pixelEnergyPtr)
{
   uint16_t  *re_orderedFrame;
   double *pixelEnergy;
   int index;

//   qDebug() << "PixelProcessor::processRe_orderFrame()";
   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   pixelEnergy = (double *) calloc(frameSize, sizeof(double));
   for (int i = 0; i < frameSize; i++)
   {
      index = pixelMap[i];
      re_orderedFrame[index] = frame[i];
      pixelEnergy[index] = re_orderedFrame[index] * gradientValue[index] + interceptValue[index];
   }
   *pixelEnergyPtr = pixelEnergy;

   return re_orderedFrame;
}

uint16_t *PixelProcessor::processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue, double **pixelEnergyPtr)
{
   uint16_t  *re_orderedFrame;
   double *pixelEnergy;
   int index;

   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   pixelEnergy = (double *) calloc(frameSize, sizeof(double));
   for (int i = 0; i < frameSize; i++)
   {
      index = pixelMap[i];
      if (frame[i] - thresholdValue < 0)
      {
         re_orderedFrame[index] = 0;
         pixelEnergy[index] = 0;
//         pixelEnergy[index] = interceptValue[index];
      }
      else
      {
         re_orderedFrame[index] = frame[i];
         pixelEnergy[index] = re_orderedFrame[index] * gradientValue[index] + interceptValue[index];
      }
   }
   *pixelEnergyPtr = pixelEnergy;

   return re_orderedFrame;
}

uint16_t *PixelProcessor::processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel, double **pixelEnergyPtr)
{
   uint16_t  *re_orderedFrame;
   double *pixelEnergy;
   int index;

   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   pixelEnergy = (double *) calloc(frameSize, sizeof(double));
   for (int i = 0; i < frameSize; i++)
   {
      index = pixelMap[i];
      if (frame[i] - thresholdPerPixel[i] < 0)
      {
         re_orderedFrame[index] = 0;
//         pixelEnergy[index] = interceptValue[index];
      }
      else
      {
         re_orderedFrame[index] = frame[i];
         pixelEnergy[index] = re_orderedFrame[index] * gradientValue[index] + interceptValue[index];
      }
   }
   *pixelEnergyPtr = pixelEnergy;

   return re_orderedFrame;
}
