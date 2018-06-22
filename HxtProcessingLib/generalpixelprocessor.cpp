#include "pixelProcessor.h"

#include <iostream>
#include <fstream>
#include <intrin.h>
#include <bitset>
#include <vector>
#include <sys/stat.h>
//
#include <QTime>
#include <qDebug>

 uint16_t GeneralPixelProcessor::pixelMap[6400];
 bool GeneralPixelProcessor::pixelMapInitialised = false;
 uint32_t GeneralPixelProcessor::frameSize = 80 * 80;

GeneralPixelProcessor::GeneralPixelProcessor()
{
   if (!GeneralPixelProcessor::pixelMapInitialised)
   {
      initialisePixelMap();
      GeneralPixelProcessor::pixelMapInitialised = true;
   }
   gradientValue = NULL;
   interceptValue = NULL;
}

void GeneralPixelProcessor::initialisePixelMap()
{
   int pmIndex = 0;

   for (int row = 0; row < 80; row++)
   {
      for (int col = 0; col < 20; col++)
      {
         for (int pix = 0; pix < 80; pix+=20)
         {
            GeneralPixelProcessor::pixelMap[pmIndex] = pix + col +(row * 80);
            pmIndex++;
         }
      }
   }
}

void GeneralPixelProcessor::setEnergyCalibration(bool energyCalibration)
{
   this->energyCalibration = energyCalibration;
}

void GeneralPixelProcessor::setLastFrameCorrection(bool lastFrameCorrection)
{
   this->lastFrameCorrection = lastFrameCorrection;
}

void GeneralPixelProcessor::setGradientValue(double *gradientValue)
{
   this->gradientValue = gradientValue;
}

void GeneralPixelProcessor::setInterceptValue(double *interceptValue)
{
   this->interceptValue = interceptValue;
}

void GeneralPixelProcessor::initialiseEnergyCalibration(double *gradientValue, double *interceptValue)
{
   this->gradientValue = gradientValue;
   this->interceptValue = interceptValue;
}

void GeneralPixelProcessor::setFrameSize(int frameSize)
{
   GeneralPixelProcessor::frameSize = frameSize;
}

void GeneralPixelProcessor::resetLastRe_orderedSize()
{
   /// Function declaration required to support 400x400 pixel processing
   /*qDebug() << "Doing nothing in: <<" Q_FUNC_INFO*/;
}
double *GeneralPixelProcessor::getGradientValue()
{
   return gradientValue;
}

double *GeneralPixelProcessor::getInterceptValue()
{
   return interceptValue;
}

uint16_t *GeneralPixelProcessor::processFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                              uint16_t *frame)
{
   uint16_t  *re_orderedFrame;
   pixelRawValMapPtr = NULL;
   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                              uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t  *re_orderedFrame;
   pixelRawValMapPtr = NULL;

   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] < thresholdValue)
      {
         re_orderedFrame[i] = 0;
      }
   }

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processFrame(unordered_map<int, double> **pixelRawValMapPtr,
                                              uint16_t *frame, uint16_t *thresholdPerPixel)
{
    uint16_t  *re_orderedFrame;
    pixelRawValMapPtr = NULL;

   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] < thresholdPerPixel[i])
      {
         re_orderedFrame[i] = 0;
      }
   }

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processFrame(uint16_t *frame,
                                              unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   pixelEnergyMapPtr = NULL;
   double value;

   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] != 0)
      {
         value = (re_orderedFrame[i] * gradientValue[i] + interceptValue[i]);
         re_orderedFrame[i] = value;
      }
   }

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processFrame(uint16_t *frame, uint16_t thresholdValue,
                                              unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   pixelEnergyMapPtr = NULL;
   double value;

   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));
//   QTime qtTime;
//   int /*copyTime = 0,*/ applyTime = 0/*, storeTime = 0*/;

   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));
//   qtTime.restart();
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] < thresholdValue)
      {
         re_orderedFrame[i] = 0;
      }
      else
      {
         value = (re_orderedFrame[i] * gradientValue[i] + interceptValue[i]);
         re_orderedFrame[i] = value;
      }
   }
//   applyTime = qtTime.elapsed();
//   qDebug() << "finished calibration, i: " << i << " frameSize: " << GeneralPixelProcessor::frameSize;
//   qDebug() << "GPP  memcopy: " << copyTime << " ms";
//   qDebug() << "GPP Calibrat: " << (applyTime) << " ms.";

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                              unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   pixelEnergyMapPtr = NULL;
   double value;

   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] < thresholdPerPixel[i])
      {
         re_orderedFrame[i] = 0;
      }
      else
      {
         value = (re_orderedFrame[i] * gradientValue[i] + interceptValue[i]);
         re_orderedFrame[i] = value;
      }
   }

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processRe_orderFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                                      uint16_t *frame)
{
   uint16_t  *re_orderedFrame;
   pixelRawValMapPtr = NULL;
   int index;

   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      index = GeneralPixelProcessor::pixelMap[i];
      re_orderedFrame[index] = frame[i];
   }

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processRe_orderFrame(unordered_map<int, double>**pixelRawValMapPtr, 
                                                      uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t  *re_orderedFrame;
   pixelRawValMapPtr = NULL;
   int index;

   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   for (unsigned int i = 0; i < frameSize; i++)
   {
      index = GeneralPixelProcessor::pixelMap[i];
      if (frame[i] < thresholdValue)
      {
         re_orderedFrame[index] = 0;
      }
      else
      {
         re_orderedFrame[index] = frame[i];
      }
   }

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processRe_orderFrame(unordered_map<int, double>**pixelRawValMapPtr, 
                                                      uint16_t *frame, uint16_t *thresholdPerPixel)
{
   uint16_t  *re_orderedFrame;
   pixelRawValMapPtr = NULL;
   int index;

   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   for (unsigned int i = 0; i < frameSize; i++)
   {
      index = pixelMap[i];
      if (frame[i] < thresholdPerPixel[i])
      {
         re_orderedFrame[index] = 0;
      }
      else
      {
         re_orderedFrame[index] = frame[i];
      }
   }

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processRe_orderFrame(uint16_t *frame,
                                                      unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   pixelEnergyMapPtr = NULL;
   double value;
   int index;

   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      index = GeneralPixelProcessor::pixelMap[i];
      re_orderedFrame[index] = frame[i];
      if (re_orderedFrame[index] != 0)
      {
         value = (re_orderedFrame[index] * gradientValue[index] + interceptValue[index]);
         re_orderedFrame[index] = value;
      }
   }

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
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
      if (frame[i] < thresholdValue)
      {
         re_orderedFrame[index] = 0;
      }
      else
      {
         re_orderedFrame[index] = frame[i];
         value = re_orderedFrame[index] * gradientValue[index] + interceptValue[index];
         re_orderedFrame[index] = value;
     }
   }

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
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
      if (frame[i] < thresholdPerPixel[i])
      {
         re_orderedFrame[index] = 0;
      }
      else
      {
         re_orderedFrame[index] = frame[i];
         value = re_orderedFrame[index] * gradientValue[index] + interceptValue[index];
         re_orderedFrame[index] = value;
      }
   }

   return re_orderedFrame;
}
