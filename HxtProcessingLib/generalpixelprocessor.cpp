#include "pixelProcessor.h"

#include <iostream>
#include <fstream>
#include <intrin.h>
#include <bitset>
#include <vector>
#include <sys/stat.h>
//
#include <QDebug>

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
   unordered_map<int, double> *pixelRawValMap;

   pixelRawValMap = new unordered_map<int, double>();
   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] != 0)
      {
         pixelRawValMap->insert(std::make_pair(i, re_orderedFrame[i]));
      }
   }
   *pixelRawValMapPtr = pixelRawValMap;

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                              uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelRawValMap;

   pixelRawValMap = new unordered_map<int, double>();
   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));
/*   qDebug() << "MODIFY TRANSFER BUFFER!!!";
   for (int i = 0; i < 160000; i += 401)
   {
      re_orderedFrame[i]= 150;
   }
   */
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] < thresholdValue)
      {
         re_orderedFrame[i] = 0;
      }
      else
      {
         pixelRawValMap->insert(std::make_pair(i, re_orderedFrame[i]));
      }
   }
   *pixelRawValMapPtr = pixelRawValMap;

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processFrame(unordered_map<int, double> **pixelRawValMapPtr,
                                              uint16_t *frame, uint16_t *thresholdPerPixel)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelRawValMap;
//   int index;

   pixelRawValMap = new unordered_map<int, double>();
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
         pixelRawValMap->insert(std::make_pair(i, re_orderedFrame[i]));
      }
   }
   *pixelRawValMapPtr = pixelRawValMap;

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processFrame(uint16_t *frame,
                                              unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelEnergyMap;
   double value;

   pixelEnergyMap = new unordered_map<int, double>();
   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] != 0)
      {
         value = (re_orderedFrame[i] * gradientValue[i] + interceptValue[i]);
         pixelEnergyMap->insert(std::make_pair(i, value));
      }
   }
   *pixelEnergyMapPtr = pixelEnergyMap;

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processFrame(uint16_t *frame, uint16_t thresholdValue,
                                              unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelEnergyMap;
   double value;
//    qDebug() << "MASTER br's ::processFrame()";
//    int nonZeroCount = 0, pixelRow = 0, pixelCol = 0;
   pixelEnergyMap = new unordered_map<int, double>();
   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   memcpy(re_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (re_orderedFrame[i] < thresholdValue)
      {
         re_orderedFrame[i] = 0;
      }
      else
      {
         value = (re_orderedFrame[i] * gradientValue[i] + interceptValue[i]);
         pixelEnergyMap->insert(std::make_pair(i, value));

//         if ((frame[i] == 0) || (i > 1410))
//             continue;
//         if (((nonZeroCount % 4) == 0) && (nonZeroCount != 0))
//             std::cout << endl;
//         std::cout << "F[" << i << "] = " << frame[i] << " \t\t";
//         nonZeroCount++;
//         if ((frame[i] > 109) && (frame[i] < 120))
//         {
//            pixelRow = (int) (i / 400);
//            pixelCol = (int) (i - (pixelRow * 400));

//            std::cout << "F[" << i << "] = \t" << frame[i] <<  "\ti.e. row: " << pixelRow << "\tcolumn: " << pixelCol << endl;
//            nonZeroCount++;
//         }
      }
   }
//   std::cout << "Found " << nonZeroCount << " hits between 110-120\n";
   *pixelEnergyMapPtr = pixelEnergyMap;

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                              unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelEnergyMap;
   double value;

   pixelEnergyMap = new unordered_map<int, double>();
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
         pixelEnergyMap->insert(std::make_pair(i, value));
      }
   }
   *pixelEnergyMapPtr = pixelEnergyMap;

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processRe_orderFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                                      uint16_t *frame)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelRawValMap;
   int index;

   pixelRawValMap = new unordered_map<int, double>();
   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      index = GeneralPixelProcessor::pixelMap[i];
      re_orderedFrame[index] = frame[i];
      if (re_orderedFrame[index] != 0)
      {
         pixelRawValMap->insert(std::make_pair(index, re_orderedFrame[index]));
      }
   }

   *pixelRawValMapPtr = pixelRawValMap;

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processRe_orderFrame(unordered_map<int, double>**pixelRawValMapPtr, 
                                                      uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelRawValMap;
   int index;

   pixelRawValMap = new unordered_map<int, double>();
   re_orderedFrame = (uint16_t *) calloc(frameSize, sizeof(uint16_t));

   for (unsigned int i = 0; i < frameSize; i++)
   {
      index = GeneralPixelProcessor::pixelMap[i];
      if (frame[i] < thresholdValue)
      {
         re_orderedFrame[index] =0;
      }
      else
      {
         re_orderedFrame[index] = frame[i];
         pixelRawValMap->insert(std::make_pair(index, re_orderedFrame[index]));
      }
   }
   *pixelRawValMapPtr = pixelRawValMap;

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processRe_orderFrame(unordered_map<int, double>**pixelRawValMapPtr, 
                                                      uint16_t *frame, uint16_t *thresholdPerPixel)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelRawValMap;
   int index;

   pixelRawValMap = new unordered_map<int, double>();
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
         pixelRawValMap->insert(std::make_pair(index, re_orderedFrame[index]));
      }
   }
   *pixelRawValMapPtr = pixelRawValMap;

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processRe_orderFrame(uint16_t *frame,
                                                      unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelEnergyMap;
   double value;
   int index;

   pixelEnergyMap = new unordered_map<int, double>();
   re_orderedFrame = (uint16_t *) calloc(GeneralPixelProcessor::frameSize, sizeof(uint16_t));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      index = GeneralPixelProcessor::pixelMap[i];
      re_orderedFrame[index] = frame[i];
      if (re_orderedFrame[index] != 0)
      {
         value = (re_orderedFrame[index] * gradientValue[index] + interceptValue[index]);
         pixelEnergyMap->insert(std::make_pair(index, value));
      }
   }

   *pixelEnergyMapPtr = pixelEnergyMap;

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
						      unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelEnergyMap;
   int index;
   double value;

   pixelEnergyMap = new unordered_map<int, double>();
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
         pixelEnergyMap->insert(std::make_pair(index, value));
//         qDebug() << "INSERTING PIXEL: " << index << value;
     }
   }
   *pixelEnergyMapPtr = pixelEnergyMap;

   return re_orderedFrame;
}

uint16_t *GeneralPixelProcessor::processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
						      unordered_map<int, double>**pixelEnergyMapPtr)
{
   uint16_t  *re_orderedFrame;
   unordered_map<int, double> *pixelEnergyMap;
   int index;
   double value;

   pixelEnergyMap = new unordered_map<int, double>();
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
         pixelEnergyMap->insert(std::make_pair(index, value));
      }
   }

   *pixelEnergyMapPtr = pixelEnergyMap;

   return re_orderedFrame;
}
