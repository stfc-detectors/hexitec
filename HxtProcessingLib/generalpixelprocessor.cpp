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
   ///
   debugFrameCounter = 0;
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
   ///
//   std::ostringstream fileContents;
//   fileContents << "------------------------------------------ frame " <<
//                            debugFrameCounter << " ------------------------------------------\n";
//   for (int i = 0; i < GeneralPixelProcessor::frameSize; i++ )
//   {
//      if(re_orderedFrame[i] > 0)
//         fileContents << "frame[" << i << "] = " << re_orderedFrame[i] << "\n";
//   }
//   std::string s  = fileContents.str();
//   writeFile(s.c_str(), s.length());
//   debugFrameCounter += 1;
   ///

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

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      // re_orderedFrame is all the zeros, so need only concern ourselves with values above thresholdValue
      if (frame[i] >= thresholdValue)
      {
         re_orderedFrame[i] = ( ((double)frame[i]) * gradientValue[i] + interceptValue[i]);
      }
   }

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
   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (frame[i] != 0)
      {
         index = GeneralPixelProcessor::pixelMap[i];
         re_orderedFrame[index] = ( ((double)frame[i]) * gradientValue[index] + interceptValue[index]);
      }
   }
   ///
//   std::ostringstream fileContents;
//   fileContents << "------------------------------------------ frame " <<
//                            debugFrameCounter << " ------------------------------------------\n";
//   for (int i = 0; i < GeneralPixelProcessor::frameSize; i++ )
//   {
//      if(re_orderedFrame[i] > 0)
//         fileContents << "Cal[" << i << "] = " << re_orderedFrame[i] << "\n";
//   }
//   std::string s  = fileContents.str();
//   writeFile(s.c_str(), s.length(), "All_540_frames_");
//   debugFrameCounter += 1;
   ///

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

//void GeneralPixelProcessor::writeFile(const char *buffer, unsigned long length, std::string filePrefix)
//{
//   std::string fname = filePrefix + /*std::to_string(debugFrameCounter) +*/ std::string("_HexitecGigE_Cal_detailed.txt");
//   outFile.open(fname.c_str(), std::ofstream::app);
//   outFile.write((const char *)buffer, length * sizeof(char));
//   outFile.close();
//}
