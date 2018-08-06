#include "pixelProcessor.h"

#include <iostream>
#include <fstream>
#include <intrin.h>
#include <bitset>
#include <vector>
#include <sys/stat.h>
//
#include <QTime>
#include <QThread>
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

void GeneralPixelProcessor::resetLastRe_orderedSize()
{
   /// Function declaration required to support 400x400 pixel processing
   ///   (For test.exe application only)
}

double *GeneralPixelProcessor::getGradientValue()
{
   return gradientValue;
}

double *GeneralPixelProcessor::getInterceptValue()
{
   return interceptValue;
}

double *GeneralPixelProcessor::processFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                              uint16_t *frame)
{
   double *re_orderedFrame;
   pixelRawValMapPtr = NULL;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (frame[i] > 0)
      {
         re_orderedFrame[i] = (double)frame[i];
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                              uint16_t *frame, uint16_t thresholdValue)
{
   double  *re_orderedFrame;
   pixelRawValMapPtr = NULL;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize *sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (frame[i] >= thresholdValue)
      {
         re_orderedFrame[i] = (double)frame[i];
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processFrame(unordered_map<int, double> **pixelRawValMapPtr,
                                              uint16_t *frame, uint16_t *thresholdPerPixel)
{
    double *re_orderedFrame;
    pixelRawValMapPtr = NULL;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(double));

   // re_orderedFrame completely empty, if a pixel in frame meets/exceeds its threshold,
   //    copy that value into re_orderedFrame's pixel
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (frame[i] >= thresholdPerPixel[i])
      {
         re_orderedFrame[i] = (double)frame[i];
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processFrame(uint16_t *frame,
                                              unordered_map<int, double>**pixelEnergyMapPtr)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = NULL;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize *  sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (frame[i] != 0)
      {
         re_orderedFrame[i] = ( ((double)frame[i]) * gradientValue[i] + interceptValue[i]);
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processFrame(uint16_t *frame, uint16_t thresholdValue,
                                              unordered_map<int, double>**pixelEnergyMapPtr)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = NULL;

   // Reordered frame will be array of double(s) (not int(s))
   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(double));

//   QTime qtTime;
//   int applyTime = 0;

//   qtTime.restart();
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      // re_orderedFrame is all the zeros, so need only concern ourselves with values above thresholdValue
      if (frame[i] >= thresholdValue)
      {
         re_orderedFrame[i] = ( ((double)frame[i]) * gradientValue[i] + interceptValue[i]);
      }
   }
//   applyTime = qtTime.elapsed();
//   qDebug() << "finished calibration, i: " << i << " frameSize: " << GeneralPixelProcessor::frameSize;
//   qDebug() << "GPP Calibrat: " << (applyTime) << " ms.";

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                              unordered_map<int, double>**pixelEnergyMapPtr)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = NULL;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(double));

   // re_orderedFrame empty, if frame's pixel meets/exceeds its threshold,
   //    calibrate and place that value in re_ orderedFrame's pixel
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (frame[i] >= thresholdPerPixel[i])
      {
         re_orderedFrame[i] = ((double)(frame[i]) * gradientValue[i] + interceptValue[i]);
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processRe_orderFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                                      uint16_t *frame)
{
   double *re_orderedFrame;
   pixelRawValMapPtr = NULL;
   int index;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (frame[i] > 0)
      {
         index = GeneralPixelProcessor::pixelMap[i];
         re_orderedFrame[index] = (double)frame[i];
      }

   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processRe_orderFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                                      uint16_t *frame, uint16_t thresholdValue)
{
   double  *re_orderedFrame;
   pixelRawValMapPtr = NULL;
   int index;

   re_orderedFrame  = (double *)  malloc(frameSize * sizeof(double));
   memset(re_orderedFrame, 0, frameSize * sizeof(double));

   for (unsigned int i = 0; i < frameSize; i++)
   {
//      index = GeneralPixelProcessor::pixelMap[i];
//      if (frame[i] < thresholdValue)
//      {
//         re_orderedFrame[index] = 0;
//      }
//      else
//      {
//         re_orderedFrame[index] = frame[i];
//      }
      if (frame[i] >= thresholdValue)
      {
         index = GeneralPixelProcessor::pixelMap[i];
         re_orderedFrame[index] = (double)frame[i];
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processRe_orderFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                                      uint16_t *frame, uint16_t *thresholdPerPixel)
{
   double *re_orderedFrame;
   pixelRawValMapPtr = NULL;
   int index;

   re_orderedFrame = (double *) malloc(frameSize * sizeof(double));
   memset(re_orderedFrame, 0, frameSize * sizeof(double));

   for (unsigned int i = 0; i < frameSize; i++)
   {
      if (frame[i] >= thresholdPerPixel[i])
      {
         index = pixelMap[i];
         re_orderedFrame[index] = (double)frame[i];
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processRe_orderFrame(uint16_t *frame,
                                                      unordered_map<int, double>**pixelEnergyMapPtr)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = NULL;
   int index;
//   qDebug() << Q_FUNC_INFO;
   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
//     if (((i > 0) && (i < 4)) &&  (frame[i] != 0))
//       std::cout << " frame[" << i << "] = " << frame[i]  << " (double)frame: " << (double)frame[i];

      if (frame[i] != 0)
      {
         index = GeneralPixelProcessor::pixelMap[i];
         re_orderedFrame[index] = ( ((double)frame[i]) * gradientValue[index] + interceptValue[index]);
//         if ((i > 0) && (i < 4))
//            std::cout << " re_orderedFrame[" << index << "]  = " << re_orderedFrame[index] << "  = " << (double)frame[i]
//                      << " * " << gradientValue[index] << " + " << interceptValue[index] << endl;
      }
//      else
//         if ((i > 0) && (i < 4))
//            std::cout << endl;
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
						      unordered_map<int, double>**pixelEnergyMapPtr)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = NULL;
   int index;

   re_orderedFrame = (double *)  malloc(GeneralPixelProcessor::frameSize *  sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (frame[i] >= thresholdValue)
      {
         index = GeneralPixelProcessor::pixelMap[i];
         re_orderedFrame[index] = (double)(frame[i]) * gradientValue[index] + interceptValue[index];
     }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
						      unordered_map<int, double>**pixelEnergyMapPtr)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = NULL;
   int index;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (frame[i] >= thresholdPerPixel[i])
      {
         index = GeneralPixelProcessor::pixelMap[i];
         re_orderedFrame[index] = ((double)frame[i]) * gradientValue[index] + interceptValue[index];
      }
   }

   return re_orderedFrame;
}
