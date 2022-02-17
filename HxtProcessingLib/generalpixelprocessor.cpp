#include "pixelProcessor.h"

#include <iostream>
#include <fstream>
#include <intrin.h>
#include <bitset>
#include <vector>
#include <sys/stat.h>
#include <QDebug>

 /// Try with same sized frame in, frame out (before translating 1600 pixels into 64..)
 uint16_t GeneralPixelProcessor::pixelMap[1600];
//  uint16_t GeneralPixelProcessor::pixelMap[1600];//[64];
 bool GeneralPixelProcessor::pixelMapInitialised = false;
 uint32_t GeneralPixelProcessor::nInRows = 20;
 uint32_t GeneralPixelProcessor::nInColumns = 80;
 uint32_t GeneralPixelProcessor::frameInSize = GeneralPixelProcessor::nInRows * GeneralPixelProcessor::nInColumns;
 uint32_t GeneralPixelProcessor::nOutRows = 20;//4;
 uint32_t GeneralPixelProcessor::nOutColumns = 80;//16;
 uint32_t GeneralPixelProcessor::frameOutSize = GeneralPixelProcessor::nOutRows * GeneralPixelProcessor::nOutColumns;
/// DEBUGGING:
#include <iostream>
#include <iomanip>
GeneralPixelProcessor::GeneralPixelProcessor(int occupancyThreshold)
{
   if (!GeneralPixelProcessor::pixelMapInitialised)
   {
      initialisePixelMap();
      GeneralPixelProcessor::pixelMapInitialised = true;
   }
   gradientValue = nullptr;
   interceptValue = nullptr;
   this->occupancyThreshold = occupancyThreshold;
   occupancyCorrections = 0;
   ///
   debugFrameCounter = 0;
}

void GeneralPixelProcessor::initialisePixelMap()
{
   uint16_t pmIndex = 0, col = 0;

   for (uint16_t row = 0; row < GeneralPixelProcessor::nInRows; row++)
   {
      for (uint16_t j = 0; j < 20; j++)
      {
         for (uint16_t k = 0; k < 80; k+=20)
         {
            col = j + k;
            GeneralPixelProcessor::pixelMap[pmIndex] = (row*80)+col;
            pmIndex++;
         }
      }
   }
//   for (uint16_t index = 0; index < 1600; index++)
//   {
//      if (index < 6*80)
//         qDebug() << " *pixelMap[" << index <<"] = " << pixelMap[index];
//   }
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

void GeneralPixelProcessor::setFrameInSize(uint32_t frameSize)
{
   GeneralPixelProcessor::frameInSize = frameSize;
}

void GeneralPixelProcessor::setFrameInCols(uint32_t columns)
{
   GeneralPixelProcessor::nInColumns = columns;
}

void GeneralPixelProcessor::setFrameInRows(uint32_t Rows)
{
   GeneralPixelProcessor::nInRows = Rows;
}

void GeneralPixelProcessor::setFrameOutSize(uint32_t frameSize)
{
   GeneralPixelProcessor::frameOutSize = frameSize;
}

void GeneralPixelProcessor::setFrameOutCols(uint32_t columns)
{
   GeneralPixelProcessor::nInColumns = columns;
}

void GeneralPixelProcessor::setFrameOutRows(uint32_t Rows)
{
   GeneralPixelProcessor::nInRows = Rows;
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
                                            uint16_t *frame, unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   pixelRawValMapPtr = nullptr;
   /// Test implementing occupancy
   int rowEventsAboveThreshold = 0;
   bool bClearRowOnce = true;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameOutSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      if (i % GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      if (frame[i] > 0)
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               bClearRowOnce = false;
            }
         }
         else
         {
            re_orderedFrame[i] = (double)frame[i];
            (*eventsInFrame)++;
         }
         rowEventsAboveThreshold++;
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                            uint16_t *frame, uint16_t thresholdValue,
                                            unsigned int *eventsInFrame)
{
   double  *re_orderedFrame;
   pixelRawValMapPtr = nullptr;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0;
   bool bClearRowOnce = true;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameOutSize *sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      if (i % GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      if (frame[i] >= thresholdValue)
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               bClearRowOnce = false;
            }
         }
         else
         {
            re_orderedFrame[i] = (double)frame[i];
            (*eventsInFrame)++;
         }
         rowEventsAboveThreshold++;
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processFrame(unordered_map<int, double> **pixelRawValMapPtr,
                                            uint16_t *frame, uint16_t *thresholdPerPixel,
                                            unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   pixelRawValMapPtr = nullptr;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0;
   bool bClearRowOnce = true;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameOutSize * sizeof(double));

   // re_orderedFrame completely empty, if a pixel in frame meets/exceeds its threshold,
   //    copy that value into re_orderedFrame's pixel
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      if (i %  GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      if (frame[i] >= thresholdPerPixel[i])
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               bClearRowOnce = false;
            }
         }
         else
         {
            re_orderedFrame[i] = (double)frame[i];
            (*eventsInFrame)++;
         }
         rowEventsAboveThreshold++;
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processFrame(uint16_t *frame,
                                            unordered_map<int, double>**pixelEnergyMapPtr,
                                            unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = nullptr;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0;
   bool bClearRowOnce = true;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameOutSize *  sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      if (i % GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      if (frame[i] != 0)
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);    // Clear the current row, Move i to end the current row
               bClearRowOnce = true;
            }
         }
         else
         {
            re_orderedFrame[i] = ( ((double)frame[i]) * gradientValue[i] + interceptValue[i]);
            (*eventsInFrame)++;
         }

         rowEventsAboveThreshold++;
      }
   }

   return re_orderedFrame;
}

/// Accepts pointer to re_orderedFrame and index i to current pixel in the entire frame.
///     Determine first and final pixel in current row, zero all pixels in current row
///     and return index of final pixel (because calling function will increment index
///     and thus carry out next operation on first pixel of the next row)
unsigned int GeneralPixelProcessor::clearCurrentRow(double *re_orderedFrame, unsigned int i)
{
   unsigned int rowIndex = i / GeneralPixelProcessor::nInRows;            // Workout which row with integer division
   unsigned int iStart = rowIndex * GeneralPixelProcessor::nInColumns;    // Find index within whole frame

   unsigned int iEnd = iStart + (GeneralPixelProcessor::nInColumns-1);
   unsigned int index = iStart;

   for ( ; index < iEnd; index++)
   {
      re_orderedFrame[index] = 0;
   }
   occupancyCorrections++;

   return iEnd;
}

void GeneralPixelProcessor::clearCurrentRow(uint16_t *re_orderedFrame, unsigned int i)
{
   unsigned int rowIndex = i / GeneralPixelProcessor::nInRows;            // Workout which row with integer division
   unsigned int iStart = rowIndex * GeneralPixelProcessor::nInColumns;    // Find index within whole frame

   unsigned int iEnd = iStart + (GeneralPixelProcessor::nInColumns-1);
   unsigned int index = iStart;

   for ( ; index < iEnd; index++)
   {
      re_orderedFrame[index] = 0;
   }
}


double *GeneralPixelProcessor::processFrame(uint16_t *frame, uint16_t thresholdValue,
                                            unordered_map<int, double>**pixelEnergyMapPtr,
                                            unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = nullptr;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0;
   bool bClearRowOnce = true;

   // Reordered frame will be array of double(s) (not int(s))
   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameOutSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      if (i % GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      // re_orderedFrame is all the zeros, so need only concern ourselves with values above thresholdValue
      if (frame[i] >= thresholdValue)
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               bClearRowOnce = false;
            }
         }
         else
         {
            re_orderedFrame[i] = ( ((double)frame[i]) * gradientValue[i] + interceptValue[i]);
            (*eventsInFrame)++;
         }
         rowEventsAboveThreshold++;
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                            unordered_map<int, double>**pixelEnergyMapPtr,
                                            unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = nullptr;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0;
   bool bClearRowOnce = true;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameOutSize * sizeof(double));

   // re_orderedFrame empty, if frame's pixel meets/exceeds its threshold,
   //    calibrate and place that value in re_ orderedFrame's pixel
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      if (i % GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      if (frame[i] >= thresholdPerPixel[i])
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               bClearRowOnce = false;
            }
         }
         else
         {
            re_orderedFrame[i] = ((double)(frame[i]) * gradientValue[i] + interceptValue[i]);
            (*eventsInFrame)++;
         }
         rowEventsAboveThreshold++;
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processRe_orderFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                                    uint16_t *frame, unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   pixelRawValMapPtr = nullptr;
   int index;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0;
   bool bClearRowOnce = true;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameOutSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      if (i % GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      if (frame[i] > 0)
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               bClearRowOnce = false;
            }
         }
         else
         {
            index = GeneralPixelProcessor::pixelMap[i];
            re_orderedFrame[index] = (double)frame[i];
            (*eventsInFrame)++;
         }
         rowEventsAboveThreshold++;
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processRe_orderFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                                    uint16_t *frame, uint16_t thresholdValue,
                                                    unsigned int *eventsInFrame)
{
   double  *re_orderedFrame;
   pixelRawValMapPtr = nullptr;
   int index;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0;
   bool bClearRowOnce = true;

   re_orderedFrame  = (double *)  malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameOutSize * sizeof(double));
//   std::cout << "\n____________________________________________________________________________________\n";
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
//      if (i < 80*7)
//      {
//          if (i % GeneralPixelProcessor::nInColumns == 0)
//              std::cout << "\n____(" << std::setw(0) << i << ")_____________\n";
//          else if (i % GeneralPixelProcessor::nInColumns == 25)
//              std::cout << "\n";
//          std::cout << std::setw(3) << GeneralPixelProcessor::pixelMap[i] << " = " << std::setw(5) << frame[i] << std::endl;
//      }

      if (i % GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      if (frame[i] >= thresholdValue)
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               bClearRowOnce = false;
            }
         }
         else
         {
            index = GeneralPixelProcessor::pixelMap[i];
            re_orderedFrame[index] = (double)frame[i];
            (*eventsInFrame)++;
         }
         rowEventsAboveThreshold++;
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processRe_orderFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                                    uint16_t *frame, uint16_t *thresholdPerPixel,
                                                    unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   pixelRawValMapPtr = nullptr;
   int index;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0;
   bool bClearRowOnce = true;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameOutSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      if (i % GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      if (frame[i] >= thresholdPerPixel[i])
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               bClearRowOnce = false;
            }
         }
         else
         {
            index = pixelMap[i];
            re_orderedFrame[index] = (double)frame[i];
            (*eventsInFrame)++;
         }
         rowEventsAboveThreshold++;
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processRe_orderFrame(uint16_t *frame,
                                                    unordered_map<int, double>**pixelEnergyMapPtr,
                                                    unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = nullptr;
   int index = 0;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0;
   bool bClearRowOnce = true;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameOutSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
       if (i % GeneralPixelProcessor::nInColumns == 0)
       {
          rowEventsAboveThreshold = 0;
          bClearRowOnce = true;
       }

      if (frame[i] != 0)
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               bClearRowOnce = false;
            }
         }
         else
         {
            index = GeneralPixelProcessor::pixelMap[i];
            re_orderedFrame[index] = ( ((double)frame[i]) * gradientValue[index] + interceptValue[index]);
            (*eventsInFrame)++;
         }
         rowEventsAboveThreshold++;
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
                                                    unordered_map<int, double>**pixelEnergyMapPtr,
                                                    unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = nullptr;
   int index;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0;
   bool bClearRowOnce = true;

   re_orderedFrame = (double *)  malloc(GeneralPixelProcessor::frameOutSize *  sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameOutSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      if (i % GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      if (frame[i] >= thresholdValue)
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               bClearRowOnce = false;
            }
         }
         else
         {
            index = GeneralPixelProcessor::pixelMap[i];
            re_orderedFrame[index] = (double)(frame[i]) * gradientValue[index] + interceptValue[index];
            (*eventsInFrame)++;
         }
         rowEventsAboveThreshold++;
      }
   }

   return re_orderedFrame;
}

double *GeneralPixelProcessor::processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                                    unordered_map<int, double>**pixelEnergyMapPtr,
                                                    unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = nullptr;
   int index;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0;
   bool bClearRowOnce = true;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameOutSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      if (i % GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      if (frame[i] >= thresholdPerPixel[i])
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               bClearRowOnce = false;
            }
         }
         else
         {
            index = GeneralPixelProcessor::pixelMap[i];
            re_orderedFrame[index] = ((double)frame[i]) * gradientValue[index] + interceptValue[index];
            (*eventsInFrame)++;
         }
         rowEventsAboveThreshold++;
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
