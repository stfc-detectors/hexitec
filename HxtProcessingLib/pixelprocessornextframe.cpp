#include "pixelprocessornextframe.h"

#include <iostream>
#include <fstream>
#include <intrin.h>
#include <bitset>
#include <vector>
#include <sys/stat.h>

PixelProcessorNextFrame::PixelProcessorNextFrame() :
   GeneralPixelProcessor()
{
   lastRe_orderedFrame = (uint16_t *) malloc(GeneralPixelProcessor::frameSize * sizeof(uint16_t));
   memset(lastRe_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(uint16_t));
   ///
   debugFrameCounter = 0;
}

void PixelProcessorNextFrame::resetLastRe_orderedSize()
{
   free(lastRe_orderedFrame);
   lastRe_orderedFrame  = (uint16_t *) malloc(GeneralPixelProcessor::frameSize * sizeof(uint16_t));
   memset(lastRe_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(uint16_t));
}

double *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t thresholdValue)
{
   double *re_orderedFrame;
   re_orderedFrame = (double *) calloc(GeneralPixelProcessor::frameSize, sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (frame[i] < thresholdValue || lastRe_orderedFrame[i] != 0)
      {
         frame[i] = 0;
      }
      else
      {
         re_orderedFrame[i] = (double)frame[i];
      }
   }
   memcpy(lastRe_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   return re_orderedFrame;
}

double *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t *thresholdPerPixel)
{
   double *re_orderedFrame;
   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(double));

   // (Re-)use frame as processed uint16_t array (to be copied into lastRe_orderedFrame),
   //    with re_orderedFrame has processed double array
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (frame[i] < thresholdPerPixel[i] || lastRe_orderedFrame[i] != 0)
      {
         frame[i] = 0;
      }
      else
      {
         // re_orderedFrame is all 0's, copy in any pixel all that meets/exceeds threshold
         //    and wasn't hit in the previous frame
         re_orderedFrame[i] = (double)frame[i];
      }
   }
   // We now know which pixels in current frame met the threshold, copied these to
   //    lastRe_orderedFrame to compare against next frame
   memcpy(lastRe_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   return re_orderedFrame;
}


double *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t thresholdValue,
                                                unordered_map<int, double>**pixelEnergyMapPtr)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = NULL;

   // Create empty frame of type double, to contain calibrated pixels
   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (frame[i] < thresholdValue || lastRe_orderedFrame[i] != 0)
      {
         // re_orderedFrame's pixel already zero
         // But must zero frame's corresponding pixel
         frame[i] = 0;
      }
      else
      {
         re_orderedFrame[i] = ((double)frame[i]) * gradientValue[i] + interceptValue[i];
      }
   }
   // Copy current frame (now stripped of any pixel not meeting threshold/that was hit in previous frame)
   //    into lastRe_orderedFrame, as comparison for the next frame
   memcpy(lastRe_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   return re_orderedFrame;
}

double *PixelProcessorNextFrame::processFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                                unordered_map<int, double>**pixelEnergyMapPtr)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = NULL;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(double));

   // uint16_t *frame to contain uncalibrated pixels, after next frame correction applied
   //    double *re_orderedFrame to contain calibrated & corrected pixels
   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      if (frame[i] < thresholdPerPixel[i] || lastRe_orderedFrame[i] != 0)
      {
         frame[i] = 0;
      }
      else
      {
         re_orderedFrame[i] = ( ((double)frame[i]) * gradientValue[i] + interceptValue[i]);
      }
   }
   // Save copy of hit pixels to compare against next frame
   memcpy(lastRe_orderedFrame, frame, GeneralPixelProcessor::frameSize * sizeof(uint16_t));

   return re_orderedFrame;
}

double *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue)
{
   double *re_orderedFrame;
   int index;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
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
      }
   }

   return re_orderedFrame;
}

double *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel)
{
   double *re_orderedFrame;
   int index;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize *  sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
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
      }
   }
   // No need to copy wholesale to lastRe_orderedFrame, already carried out in above for loop

   return re_orderedFrame;
}

double *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
                                               unordered_map<int, double>**pixelEnergyMapPtr)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = NULL;
   int index;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      index = GeneralPixelProcessor::pixelMap[i];
      if (frame[i] < thresholdValue || lastRe_orderedFrame[index] > 0)
      {
         lastRe_orderedFrame[index] = 0;
      }
      else
      {
         re_orderedFrame[index] = ( ((double)frame[i]) * gradientValue[index] + interceptValue[index]);
         // Mark same pixel for next frame to be processed
         lastRe_orderedFrame[index] = frame[i];
     }
   }
   // No need to copy to lastRe_orderedFrame, already carried out in above for loop
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

double *PixelProcessorNextFrame::processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                                        unordered_map<int, double>**pixelEnergyMapPtr)
{
   double *re_orderedFrame;
   pixelEnergyMapPtr = NULL;
   int index;

   re_orderedFrame = (double *) malloc(GeneralPixelProcessor::frameSize * sizeof(double));
   memset(re_orderedFrame, 0, GeneralPixelProcessor::frameSize * sizeof(double));

   for (unsigned int i = 0; i < GeneralPixelProcessor::frameSize; i++)
   {
      index = GeneralPixelProcessor::pixelMap[i];
      if (frame[i] < thresholdPerPixel[index] || lastRe_orderedFrame[index] > 0)
      {
          lastRe_orderedFrame[index] = 0;
      }
      else
      {
         re_orderedFrame[index] = ( ((double)frame[i]) * gradientValue[index] + interceptValue[index]);
         // This pixel exceeded threshold, mark corresponding pixel for next frame
         lastRe_orderedFrame[index] = frame[i];
      }
   }

   return re_orderedFrame;
}

void PixelProcessorNextFrame::writeFile(const char *buffer, unsigned long length, std::string filePrefix)
{
   std::string fname = filePrefix + /*std::to_string(debugFrameCounter) +*/ std::string("_HexitecGigE_Cal_detailed.txt");
   outFile.open(fname.c_str(), std::ofstream::app);
   outFile.write((const char *)buffer, length * sizeof(char));
   outFile.close();
}
