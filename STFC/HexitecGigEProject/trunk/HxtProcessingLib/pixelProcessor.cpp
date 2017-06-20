#include "pixelProcessor.h"
#include <QDebug>

#include <iostream>
#include <fstream>
#include <intrin.h>
#include <bitset>
#include <vector>
#include <sys/stat.h>

PixelProcessor::PixelProcessor()
{
   initialisePixelMap();
   gradientValue = NULL;
   interceptValue = NULL;
}

void PixelProcessor::initialisePixelMap()
{
   int pmIndex = 0;

   frameSize = 80 * 80;
   for (int row = 0; row < 80; row++)
   {
      qDebug() << "+++++++++++ ROW = "<< row;
      for (int col = 0; col < 20; col++)
      {
         qDebug() << "=========== COL = "<< col;
         for (int pix = 0; pix < 80; pix+=20)
         {
            pixelMap[pmIndex] = pix + col +(row * 80);
            pmIndex++;
         }
      }
   }
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

uint16_t *PixelProcessor::re_orderFrame(uint16_t *frame)
{
   uint16_t  *re_orderedFrame;

   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));
   for (int i = 0; i < frameSize; i++)
   {
      re_orderedFrame[i] = frame[pixelMap[i]];
   }
   return re_orderedFrame;
}

uint16_t *PixelProcessor::re_orderFrame(uint16_t *frame, uint16_t thresholdValue, double *pixelEnergy)
{
   uint16_t  *re_orderedFrame;

   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   if (pixelEnergy == NULL)
   {
   for (int i = 0; i < frameSize; i++)
   {
      if (frame[pixelMap[i]] - thresholdValue < 0)
      {
         re_orderedFrame[i] =0;
      }
      else
      {
         re_orderedFrame[i] = frame[pixelMap[i]];
      }
   }
   }
   else
   {
      for (int i = 0; i < frameSize; i++)
      {
         if (frame[pixelMap[i]] - thresholdValue < 0)
         {
            re_orderedFrame[i] = 0;
            pixelEnergy[i] = interceptValue[i];
         }
         else
         {
            re_orderedFrame[i] = frame[pixelMap[i]];
            pixelEnergy[i] = re_orderedFrame[i] * gradientValue[i] + interceptValue[i];
         }
      }
   }

   return re_orderedFrame;
}

uint16_t *PixelProcessor::re_orderFrame(uint16_t *frame, uint16_t *thresholdPerPix,
                                        double *pixelEnergy)
{
   uint16_t  *re_orderedFrame;

   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   if (pixelEnergy == NULL)
   {
      for (int i = 0; i < frameSize; i++)
      {
         if (frame[pixelMap[i]] - thresholdPerPix[i] < 0)
         {
            re_orderedFrame[i] = 0;
         }
         else
         {
            re_orderedFrame[i] = frame[pixelMap[i]];
         }
      }
   }
   else
   {
      for (int i = 0; i < frameSize; i++)
      {
         if (frame[pixelMap[i]] - thresholdPerPix[i] < 0)
         {
            re_orderedFrame[i] = 0;
            pixelEnergy[i] = interceptValue[i];
         }
         else
         {
            re_orderedFrame[i] = frame[pixelMap[i]];
            pixelEnergy[i] = re_orderedFrame[i] * gradientValue[i] + interceptValue[i];
         }
      }
   }
   return re_orderedFrame;
}

/*
uint16_t *PixelProcessor::re_orderFrame(uint16_t *frame, uint16_t thresholdValue,
                                      double *gradientValue, double *interceptValue, double *pixelEnergy)
{
   this->gradientValue = gradientValue;
   this->interceptValue = interceptValue;
   pixelEnergy = (double *) calloc(frameSize, sizeof(double));

   return re_orderFrame(frame, thresholdValue, pixelEnergy);
}

uint16_t *PixelProcessor::re_orderFrame(uint16_t *frame, uint16_t *thresholdPerPix,
                                      double *gradientValue, double *interceptValue, double *pixelEnergy)
{
   this->gradientValue = gradientValue;
   this->interceptValue = interceptValue;
   pixelEnergy = (double *) calloc(frameSize, sizeof(double));

   return re_orderFrame(frame, thresholdPerPix, pixelEnergy);
}

*/
