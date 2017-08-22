#include "pixelprocessornextframe.h"
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


PixelProcessorNextFrame::PixelProcessorNextFrame()
{
   lastRe_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));
}

uint16_t *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t  *re_orderedFrame;
   double *pixelEnergy;

   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   qDebug() << "PixelProcessorNextFrame::processFrame() 2";
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

uint16_t *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t *thresholdPerPixel)
{
   uint16_t  *re_orderedFrame;
   double *pixelEnergy;

   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   qDebug() << "PixelProcessorNextFrame::processFrame() 3";
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


uint16_t *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t thresholdValue,
                                                unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelEnergyMap;
   double value;

   pixelEnergyMap = new unordered_map<int, double>();
   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, frameSize * sizeof(uint16_t));
   for (int i = 0; i < frameSize; i++)
   {
      if (re_orderedFrame[i] - thresholdValue < 0)
      {
         re_orderedFrame[i] = 0;
      }
      else
      {
         value = (re_orderedFrame[i] * gradientValue[i] + interceptValue[i]);
         pixelEnergyMap->insert(std::make_pair(i,value));
      }
   }
   *pixelEnergyMapPtr = pixelEnergyMap;

   return re_orderedFrame;
}

uint16_t *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                                unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelEnergyMap;
   double value;

   pixelEnergyMap = new unordered_map<int, double>();
   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, frameSize * sizeof(uint16_t));
   for (int i = 0; i < frameSize; i++)
   {
      if (re_orderedFrame[i] - thresholdPerPixel[i] < 0)
      {
         re_orderedFrame[i] = 0;
      }
      else
      {
         value = (re_orderedFrame[i] * gradientValue[i] + interceptValue[i]);
         pixelEnergyMap->insert(std::make_pair(i,value));
      }
   }
   *pixelEnergyMapPtr = pixelEnergyMap;

   return re_orderedFrame;
}

uint16_t *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue)
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

uint16_t *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel)
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
/*
uint16_t *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, double **pixelEnergyPtr)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelEnergyMap;
   double value;
   int index;

   pixelEnergyMap = new unordered_map<int, double>();
//   qDebug() << "PixelProcessorNextFrame::processRe_orderFrame()";
   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   for (int i = 0; i < frameSize; i++)
   {
      index = pixelMap[i];
      re_orderedFrame[index] = frame[i];
      pixelEnergy[index] = re_orderedFrame[index] * gradientValue[index] + interceptValue[index];
   }
   *pixelEnergyPtr = pixelEnergy;

   return re_orderedFrame;
}

uint16_t *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue, double **pixelEnergyPtr)
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
*/
uint16_t *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
                                               unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelEnergyMap;
   int index;
   double value;

   pixelEnergyMap = new unordered_map<int, double>();
   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   for (int i = 0; i < frameSize; i++)
   {
      index = pixelMap[i];
      if (frame[i] < thresholdValue || lastRe_orderedFrame[index] > 0)
      {
         re_orderedFrame[index] = 0;
      }
      else
      {
         re_orderedFrame[index] = frame[i];
         value = (re_orderedFrame[index] * gradientValue[index] + interceptValue[index]);
         pixelEnergyMap->insert(std::make_pair(index,value));
     }
   }
   *pixelEnergyMapPtr = pixelEnergyMap;
   memcpy(lastRe_orderedFrame, re_orderedFrame, frameSize * sizeof(uint16_t));

   return re_orderedFrame;
}

uint16_t *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                                        unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelEnergyMap;
   int index;
   double value;

   pixelEnergyMap = new unordered_map<int, double>();
   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   for (int i = 0; i < frameSize; i++)
   {
      index = pixelMap[i];
      if (frame[i] - thresholdPerPixel[i] < 0)
      {
         re_orderedFrame[index] = 0;
      }
      else
      {
         re_orderedFrame[index] = frame[i];
         value = (re_orderedFrame[index] * gradientValue[index] + interceptValue[index]);
         pixelEnergyMap->insert(std::make_pair(index,value));
      }
   }
   *pixelEnergyMapPtr = pixelEnergyMap;

   return re_orderedFrame;
}
