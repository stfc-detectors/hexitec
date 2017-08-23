#include "pixelprocessornextframe.h"
#include <QDebug>

#include <iostream>
#include <fstream>
#include <intrin.h>
#include <bitset>
#include <vector>
#include <sys/stat.h>

PixelProcessorNextFrame::PixelProcessorNextFrame() :
   GeneralPixelProcessor()
{
   qDebug() << "PixelProcessorNextFrame CONSTRUCTED";
   lastRe_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));
}

uint16_t *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t  *re_orderedFrame;
   double *pixelEnergy;

   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   qDebug() << "PixelProcessorNextFrame::processFrame() 2";
   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));
   for (int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] - thresholdValue < 0 || lastRe_orderedFrame[i] != 0)
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

   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   qDebug() << "PixelProcessorNextFrame::processFrame() 3";
   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));
   for (int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] - thresholdPerPixel[i] < 0 || lastRe_orderedFrame[i] != 0)
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

   qDebug() << "PixelProcessorNextFrame::processFrame() 4";
   pixelEnergyMap = new unordered_map<int, double>();
   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));
   for (int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] - thresholdValue < 0 || lastRe_orderedFrame[i] != 0)
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

   qDebug() << "PixelProcessorNextFrame::processFrame() 5";
   pixelEnergyMap = new unordered_map<int, double>();
   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));
   for (int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] - thresholdPerPixel[i] < 0 || lastRe_orderedFrame[i] != 0)
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

   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   qDebug() << "PixelProcessorNextFrame::processRe_orderFrame() called";
   for (int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (frame[i] - thresholdValue < 0 || lastRe_orderedFrame[pixelMap[i]] != 0)
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

   qDebug() << "PixelProcessorNextFrame::processFrame() 6";
   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   for (int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (frame[i] - thresholdPerPixel[i] < 0 || lastRe_orderedFrame[pixelMap[i]] != 0)
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

uint16_t *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
                                               unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelEnergyMap;
   int index;
   double value;

   pixelEnergyMap = new unordered_map<int, double>();
   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));
   qDebug() << "PixelProcessorNextFrame::processFrame() 7, frameSize = " << GeneralPixelProcessor::frameSize;

   for (int i = 0; i < GeneralPixelProcessor::frameSize; i++)
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
   memcpy(lastRe_orderedFrame, re_orderedFrame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   return re_orderedFrame;
}

uint16_t *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                                        unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelEnergyMap;
   int index;
   double value;

   qDebug() << "PixelProcessorNextFrame::processRe_orderFrame() called";

   pixelEnergyMap = new unordered_map<int, double>();
   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   for (int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      index = pixelMap[i];
      if (frame[i] - thresholdPerPixel[i] < 0 || lastRe_orderedFrame[index] > 0)
      {
         re_orderedFrame[index] = 0;
      }
      else
      {
         qDebug() << "inserting value !!!!!!!!!!!!!!!!!!";
         re_orderedFrame[index] = frame[i];
         value = (re_orderedFrame[index] * gradientValue[index] + interceptValue[index]);
         pixelEnergyMap->insert(std::make_pair(index,value));
      }
   }
   *pixelEnergyMapPtr = pixelEnergyMap;

   return re_orderedFrame;
}
