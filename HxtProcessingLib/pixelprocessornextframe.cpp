#include "pixelprocessornextframe.h"

#include <iostream>
#include <fstream>
#include <intrin.h>
#include <bitset>
#include <vector>
#include <sys/stat.h>
///
#include <QDebug>

PixelProcessorNextFrame::PixelProcessorNextFrame(int occupancyThreshold) :
   GeneralPixelProcessor(occupancyThreshold)
{
   lastRe_orderedFrame = (uint16_t *) malloc(GeneralPixelProcessor::frameInSize * sizeof(uint16_t));
   memset(lastRe_orderedFrame, 0, GeneralPixelProcessor::frameInSize * sizeof(uint16_t));
   ///
   debugFrameCounter = 0;
}

void PixelProcessorNextFrame::resetLastRe_orderedSize()
{
   free(lastRe_orderedFrame);
   lastRe_orderedFrame  = (uint16_t *) malloc(GeneralPixelProcessor::frameInSize * sizeof(uint16_t));
   memset(lastRe_orderedFrame, 0, GeneralPixelProcessor::frameInSize * sizeof(uint16_t));
}

double *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t thresholdValue,
                                              unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0;
   bool bClearRowOnce = true;
   qDebug() << "PPNF 01";
   re_orderedFrame = (double *) calloc(GeneralPixelProcessor::frameInSize, sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameInSize * sizeof(double));
   /// Construct second, to be displayed, frame
   double *displayFrame;
   uint16_t rowMin = 320, rowMax = 400, displIndex = 0;
   displayFrame = (double *) malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(displayFrame, 0, GeneralPixelProcessor::frameOutSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      if (i % GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      if (frame[i] < thresholdValue || lastRe_orderedFrame[i] != 0)
      {
         frame[i] = 0;
      }
      else
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               clearCurrentRow(frame, i);
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
   memcpy(lastRe_orderedFrame, frame, GeneralPixelProcessor::frameInSize * sizeof(uint16_t));

   /// Fill 4x16 displayed frame with selected pixels from re_orderedFrame
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      /// Update row minimum, maximum
      if ((i > 0) && (i % (GeneralPixelProcessor::nInColumns * 5) == 0))
      {
         rowMin += (GeneralPixelProcessor::nInColumns * 5);
         rowMax += (GeneralPixelProcessor::nInColumns * 5);
      }
      if ((i > rowMin) && (i < rowMax ) &&   // On row 0, 5, 10, 15, 20?
         (((i % 5) - 2) == 0))                // on col 2, 7, 12, .., 77?
      {
         displayFrame[displIndex++] = re_orderedFrame[i];
      }
   }

   return displayFrame;
}

double *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                              unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0;
   bool bClearRowOnce = true;
   qDebug() << "PPNF 02";
   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameInSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameInSize * sizeof(double));
   /// Construct second, to be displayed, frame
   double *displayFrame;
   uint16_t rowMin = 320, rowMax = 400, displIndex = 0;
   displayFrame = (double *) malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(displayFrame, 0, GeneralPixelProcessor::frameOutSize * sizeof(double));

   // (Re-)use frame as processed uint16_t array (to be copied into lastRe_orderedFrame),
   //    with re_orderedFrame has processed double array
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      if (i % GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      if (frame[i] < thresholdPerPixel[i] || lastRe_orderedFrame[i] != 0)
      {
         frame[i] = 0;
      }
      else
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               clearCurrentRow(frame, i);
               bClearRowOnce = false;
            }
         }
         else
         {
            // re_orderedFrame is all 0's, copy in any pixel all that meets/exceeds threshold
            //    and wasn't hit in the previous frame
            re_orderedFrame[i] = (double)frame[i];
            (*eventsInFrame)++;
         }
         rowEventsAboveThreshold++;
      }
   }
   // We now know which pixels in current frame met the threshold, copied these to
   //    lastRe_orderedFrame to compare against next frame
   memcpy(lastRe_orderedFrame, frame, GeneralPixelProcessor::frameInSize * sizeof(uint16_t));

   /// Fill 4x16 displayed frame with selected pixels from re_orderedFrame
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      /// Update row minimum, maximum
      if ((i > 0) && (i % (GeneralPixelProcessor::nInColumns * 5) == 0))
      {
         rowMin += (GeneralPixelProcessor::nInColumns * 5);
         rowMax += (GeneralPixelProcessor::nInColumns * 5);
      }
      if ((i > rowMin) && (i < rowMax ) &&   // On row 0, 5, 10, 15, 20?
         (((i % 5) - 2) == 0))                // on col 2, 7, 12, .., 77?
      {
         displayFrame[displIndex++] = re_orderedFrame[i];
      }
   }

   return displayFrame;
}

double *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t thresholdValue,
                                              unordered_map<int, double>**pixelEnergyMapPtr,
                                              unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = nullptr;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0, index = 0;
   bool bClearRowOnce = true;
//   qDebug() << "PPNF 03 - Modified!";
   // Create empty frame of type double, to contain calibrated pixels
   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameInSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameInSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      if (i % GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      if (frame[i] < thresholdValue || lastRe_orderedFrame[i] != 0)
      {
         // re_orderedFrame's pixel already zero
         // But must zero frame's corresponding pixel
         frame[i] = 0;
      }
      else
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               clearCurrentRow(frame, i);
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
   // Copy current frame (now stripped of any pixel not meeting threshold/that was hit in previous frame)
   //    into lastRe_orderedFrame, as comparison for the next frame
   memcpy(lastRe_orderedFrame, frame, GeneralPixelProcessor::frameInSize * sizeof(uint16_t));

   return re_orderedFrame;
}

double *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                              unordered_map<int, double>**pixelEnergyMapPtr,
                                              unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = nullptr;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0, index = 0;
   bool bClearRowOnce = true;
//   qDebug() << "PPNF 04 - Modified!";
   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameInSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameInSize * sizeof(double));

   // uint16_t *frame to contain uncalibrated pixels, after next frame correction applied
   //    double *re_orderedFrame to contain calibrated & corrected pixels
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      if (i % GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      if (frame[i] < thresholdPerPixel[i] || lastRe_orderedFrame[i] != 0)
      {
         frame[i] = 0;
      }
      else
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               clearCurrentRow(frame, i);
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
   // Save copy of hit pixels to compare against next frame
   memcpy(lastRe_orderedFrame, frame, GeneralPixelProcessor::frameInSize * sizeof(uint16_t));

   return re_orderedFrame;
}

double *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
                                                      unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   int index;
   qDebug() << "PPNF 05";
   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameInSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameInSize * sizeof(double));
   /// Construct second, to be displayed, frame
   double *displayFrame;
   uint16_t rowMin = 320, rowMax = 400, displIndex = 0;
   displayFrame = (double *) malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(displayFrame, 0, GeneralPixelProcessor::frameOutSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      index = GeneralPixelProcessor::pixelMap[i];
      if (frame[i] - thresholdValue < 0 || lastRe_orderedFrame[index] != 0)
      {
         lastRe_orderedFrame[index] = 0;
      }
      else
      {
         re_orderedFrame[index] = (double)frame[i];
         lastRe_orderedFrame[index] = frame[i];
         (*eventsInFrame)++;
      }
   }

   /// Fill 4x16 displayed frame with selected pixels from re_orderedFrame
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      /// Update row minimum, maximum
      if ((i > 0) && (i % (GeneralPixelProcessor::nInColumns * 5) == 0))
      {
         rowMin += (GeneralPixelProcessor::nInColumns * 5);
         rowMax += (GeneralPixelProcessor::nInColumns * 5);
      }
      if ((i > rowMin) && (i < rowMax ) &&   // On row 0, 5, 10, 15, 20?
         (((i % 5) - 2) == 0))                // on col 2, 7, 12, .., 77?
      {
         displayFrame[displIndex++] = re_orderedFrame[i];
      }
   }

   return displayFrame;
}

double *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                                      unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   int index;
   qDebug() << "PPNF 06";
   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameInSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameInSize * sizeof(double));
   /// Construct second, to be displayed, frame
   double *displayFrame;
   uint16_t rowMin = 320, rowMax = 400, displIndex = 0;
   displayFrame = (double *) malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(displayFrame, 0, GeneralPixelProcessor::frameOutSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      index = GeneralPixelProcessor::pixelMap[i];
      if (frame[i] - thresholdPerPixel[i] < 0 || lastRe_orderedFrame[index] != 0)
      {
         lastRe_orderedFrame[index] = 0;
      }
      else
      {
         re_orderedFrame[index] = (double)frame[i];
         // Mark same pixel for next frame to be processed
         lastRe_orderedFrame[index] = frame[i];
         (*eventsInFrame)++;
      }
   }
   // No need to copy wholesale to lastRe_orderedFrame, already carried out in above for loop

   /// Fill 4x16 displayed frame with selected pixels from re_orderedFrame
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      /// Update row minimum, maximum
      if ((i > 0) && (i % (GeneralPixelProcessor::nInColumns * 5) == 0))
      {
         rowMin += (GeneralPixelProcessor::nInColumns * 5);
         rowMax += (GeneralPixelProcessor::nInColumns * 5);
      }
      if ((i > rowMin) && (i < rowMax ) &&   // On row 0, 5, 10, 15, 20?
         (((i % 5) - 2) == 0))                // on col 2, 7, 12, .., 77?
      {
         displayFrame[displIndex++] = re_orderedFrame[i];
      }
   }

   return displayFrame;
}

double *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
                                                      unordered_map<int, double>**pixelEnergyMapPtr,
                                                      unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = nullptr;
   int index;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0;
   bool bClearRowOnce = true;
//   qDebug() << "PPNF 07";
   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameInSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameInSize * sizeof(double));
   /// Construct second, to be displayed, frame
   double *displayFrame;
   uint16_t rowMin = 320, rowMax = 400, displIndex = 0;
   displayFrame = (double *) malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(displayFrame, 0, GeneralPixelProcessor::frameOutSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      if (i % GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      index = GeneralPixelProcessor::pixelMap[i];
      if (frame[i] < thresholdValue || lastRe_orderedFrame[index] > 0)
      {
         lastRe_orderedFrame[index] = 0;
      }
      else
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               clearCurrentRow(lastRe_orderedFrame, i);
               bClearRowOnce = false;
            }
         }
         else
         {
            re_orderedFrame[index] = ( ((double)frame[i]) * gradientValue[index] + interceptValue[index]);
            // Mark same pixel for next frame to be processed
            lastRe_orderedFrame[index] = frame[i];
            (*eventsInFrame)++;
         }
         rowEventsAboveThreshold++;
      }
   }

   /// Fill 4x16 displayed frame with selected pixels from re_orderedFrame
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      /// Update row minimum, maximum
      if ((i > 0) && (i % (GeneralPixelProcessor::nInColumns * 5) == 0))
      {
         rowMin += (GeneralPixelProcessor::nInColumns * 5);
         rowMax += (GeneralPixelProcessor::nInColumns * 5);
      }
      if ((i > rowMin) && (i < rowMax ) &&   // On row 0, 5, 10, 15, 20?
         (((i % 5) - 2) == 0))                // on col 2, 7, 12, .., 77?
      {
         displayFrame[displIndex++] = re_orderedFrame[i];
      }
   }

   return displayFrame;
}

double *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                                      unordered_map<int, double>**pixelEnergyMapPtr,
                                                      unsigned int *eventsInFrame)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = nullptr;
   int index;
   /// Test implementing occupancy threshold
   int rowEventsAboveThreshold = 0;
   bool bClearRowOnce = true;
//   qDebug() << "PPNF 08";
   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameInSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameInSize * sizeof(double));
   /// Construct second, to be displayed, frame
   double *displayFrame;
   uint16_t rowMin = 320, rowMax = 400, displIndex = 0;
   displayFrame = (double *) malloc(GeneralPixelProcessor::frameOutSize * sizeof(double));
   memset(displayFrame, 0, GeneralPixelProcessor::frameOutSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      if (i % GeneralPixelProcessor::nInColumns == 0)
      {
         rowEventsAboveThreshold = 0;
         bClearRowOnce = true;
      }

      index = GeneralPixelProcessor::pixelMap[i];
      if (frame[i] < thresholdPerPixel[index] || lastRe_orderedFrame[index] > 0)
      {
          lastRe_orderedFrame[index] = 0;
      }
      else
      {
         if (rowEventsAboveThreshold >= occupancyThreshold)
         {
            if (bClearRowOnce)
            {
               i = clearCurrentRow(re_orderedFrame, i);
               clearCurrentRow(lastRe_orderedFrame, i);
               bClearRowOnce = false;
            }
         }
         else
         {
            re_orderedFrame[index] = ( ((double)frame[i]) * gradientValue[index] + interceptValue[index]);
            // This pixel exceeded threshold, mark corresponding pixel for next frame
            lastRe_orderedFrame[index] = frame[i];
            (*eventsInFrame)++;
         }
         rowEventsAboveThreshold++;
      }
   }

   /// Fill 4x16 displayed frame with selected pixels from re_orderedFrame
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameInSize; i++)
   {
      /// Update row minimum, maximum
      if ((i > 0) && (i % (GeneralPixelProcessor::nInColumns * 5) == 0))
      {
         rowMin += (GeneralPixelProcessor::nInColumns * 5);
         rowMax += (GeneralPixelProcessor::nInColumns * 5);
      }
      if ((i > rowMin) && (i < rowMax ) &&   // On row 0, 5, 10, 15, 20?
         (((i % 5) - 2) == 0))                // on col 2, 7, 12, .., 77?
      {
         displayFrame[displIndex++] = re_orderedFrame[i];
      }
   }

   return displayFrame;
}

void PixelProcessorNextFrame::writeFile(const char *buffer, unsigned long length, std::string filePrefix)
{
   std::string fname = filePrefix + /*std::to_string(debugFrameCounter) +*/ std::string("_HexitecGigE_Cal_detailed.txt");
   outFile.open(fname.c_str(), std::ofstream::app);
   outFile.write((const char *)buffer, length * sizeof(char));
   outFile.close();
}
