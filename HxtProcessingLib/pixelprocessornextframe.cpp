#include "pixelprocessornextframe.h"

#include <iostream>
#include <fstream>
#include <intrin.h>
#include <bitset>
#include <vector>
#include <sys/stat.h>
//
#include <qdebug.h>

PixelProcessorNextFrame::PixelProcessorNextFrame() :
   GeneralPixelProcessor()
{
   lastRe_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));
}

void PixelProcessorNextFrame::resetLastRe_orderedSize()
{
   free(lastRe_orderedFrame);
   lastRe_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));
}

uint16_t *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t  *re_orderedFrame;
   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] < thresholdValue || lastRe_orderedFrame[i] != 0)
      {
         re_orderedFrame[i] = 0;
      }
   }
   memcpy(lastRe_orderedFrame, re_orderedFrame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   return re_orderedFrame;
}

uint16_t *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t *thresholdPerPixel)
{
   uint16_t  *re_orderedFrame;
   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] < thresholdPerPixel[i] || lastRe_orderedFrame[i] != 0)
      {
         re_orderedFrame[i] = 0;
      }
   }
   memcpy(lastRe_orderedFrame, re_orderedFrame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   return re_orderedFrame;
}


uint16_t *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t thresholdValue,
                                                unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   double value;
   pixelEnergyMapPtr = NULL;

   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] < thresholdValue || lastRe_orderedFrame[i] != 0)
      {
         re_orderedFrame[i] = 0;
      }
      else
      {
         value = (re_orderedFrame[i] * gradientValue[i] + interceptValue[i]);
         re_orderedFrame[i] = value;
      }
   }
   memcpy(lastRe_orderedFrame, re_orderedFrame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   return re_orderedFrame;
}

uint16_t *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                                unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   pixelEnergyMapPtr = NULL;
   double value;

   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] < thresholdPerPixel[i] || lastRe_orderedFrame[i] != 0)
      {
         re_orderedFrame[i] = 0;
      }
      else
      {
         value = (re_orderedFrame[i] * gradientValue[i] + interceptValue[i]);
         re_orderedFrame[i] = value;
      }
   }
   memcpy(lastRe_orderedFrame, re_orderedFrame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   return re_orderedFrame;
}

uint16_t *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t  *re_orderedFrame;
   int index;

   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      index = GeneralPixelProcessor::pixelMap[i];
      if (frame[i] - thresholdValue < 0 || lastRe_orderedFrame[index] != 0)
      {
         re_orderedFrame[index] =0;
      }
      else
      {
         re_orderedFrame[index] = frame[i];
      }
   }
   memcpy(lastRe_orderedFrame, re_orderedFrame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   return re_orderedFrame;
}

uint16_t *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel)
{
   uint16_t  *re_orderedFrame;
   int index;

   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      index = GeneralPixelProcessor::pixelMap[i];
      if (frame[i] - thresholdPerPixel[i] < 0 || lastRe_orderedFrame[index] != 0)
      {
         re_orderedFrame[index] = 0;
      }
      else
      {
         re_orderedFrame[index] = frame[i];
      }
   }
   memcpy(lastRe_orderedFrame, re_orderedFrame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   return re_orderedFrame;
}

uint16_t *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
                                               unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   pixelEnergyMapPtr = NULL;
   int index;
   double value;

   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      index = GeneralPixelProcessor::pixelMap[i];
      if (frame[i] < thresholdValue || lastRe_orderedFrame[index] > 0)
      {
         re_orderedFrame[index] = 0;
      }
      else
      {
         re_orderedFrame[index] = frame[i];
         value = (re_orderedFrame[index] * gradientValue[index] + interceptValue[index]);
         re_orderedFrame[index] = value;
     }
   }
   memcpy(lastRe_orderedFrame, re_orderedFrame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   return re_orderedFrame;
}

uint16_t *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                                        unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   pixelEnergyMapPtr = NULL;
   int index;
   double value;

   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      index = GeneralPixelProcessor::pixelMap[i];
      if (frame[i] < thresholdPerPixel[index] || lastRe_orderedFrame[index] > 0)
      {
         re_orderedFrame[index] = 0;
      }
      else
      {
         re_orderedFrame[index] = frame[i];
         value = (re_orderedFrame[index] * gradientValue[index] + interceptValue[index]);
         re_orderedFrame[index] = value;
      }
   }
   memcpy(lastRe_orderedFrame, re_orderedFrame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   return re_orderedFrame;
}
