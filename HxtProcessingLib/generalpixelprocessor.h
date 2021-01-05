#ifndef GENREALPIXELPROCESSOR_H
#define GENREALPIXELPROCESSOR_H

#include <cstdint>
#include <string>
#include <unordered_map>
///
#include <fstream>
#include <sstream>

using namespace std;

class GeneralPixelProcessor
{

public:
   static uint16_t pixelMap[6400];
   static bool pixelMapInitialised;
   static uint32_t frameSize;
   static uint32_t nRows;
   static uint32_t nColumns;

public:
   GeneralPixelProcessor(int occupancyThreshold);
   void initialiseEnergyCalibration(double *gradientValue, double *interceptValue);
   void setFrameSize(int frameSize);
   void setCols(int columns);
   void setRows(int rows);
   void setEnergyCalibration(bool energyCalibration);
   void setLastFrameCorrection(bool lastFrameCorrection);
   void setGradientValue(double *gradientValue);
   void setInterceptValue(double *gradientValue);
   double *getGradientValue();
   double *getInterceptValue();
   /// Need to change PixelProcessorFrame::lastRe_ordered size, but as member not part of parent,
   ///  and PixelProcessorFrame object only known at program execution, we need a virtual function here
   virtual void resetLastRe_orderedSize();
   ///

   double *processFrame(unordered_map<int, double>**pixelRawValMapPtr,
                        uint16_t *frame, unsigned int *eventsInFrame);
   virtual double *processFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                uint16_t *frame, uint16_t thresholdValue,
                                unsigned int *eventsInFrame);
   virtual double *processFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                uint16_t *frame, uint16_t *thresholdPerPixel,
                                unsigned int *eventsInFrame);

   virtual double *processFrame(uint16_t *frame,
                                unordered_map<int, double>**pixelEnergyMapPtr,
                                unsigned int *eventsInFrame);
   virtual double *processFrame(uint16_t *frame, uint16_t thresholdValue,
                                unordered_map<int, double>**pixelEnergyMapPtr,
                                unsigned int *eventsInFrame);
   virtual double *processFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                unordered_map<int, double>**pixelEnergyMapPtr,
                                unsigned int *eventsInFrame);

   double *processRe_orderFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                uint16_t *frame, unsigned int *eventsInFrame);
   virtual double *processRe_orderFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                        uint16_t *frame, uint16_t thresholdValue,
                                        unsigned int *eventsInFrame);
   virtual double *processRe_orderFrame(unordered_map<int, double>**pixelRawValMapPtr,
                                        uint16_t *frame, uint16_t *thresholdPerPixel,
                                        unsigned int *eventsInFrame);

   double *processRe_orderFrame(uint16_t *frame,
                  unordered_map<int, double>**pixelEnergyMapPtr, unsigned int *eventsInFrame);
   virtual double *processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
                                        unordered_map<int, double>**pixelEnergyMapPtr,
                                        unsigned int *eventsInFrame);
   virtual double *processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
                                        unordered_map<int, double>**pixelEnergyMapPtr,
                                        unsigned int *eventsInFrame);

   virtual int getOccupancyCorrections() { return occupancyCorrections; }

protected:
   void initialisePixelMap();
   bool energyCalibration;
   bool lastFrameCorrection;
   bool chargedSharingCorrection;
   double *gradientValue;
   double *interceptValue;
   uint16_t *lastFrame;
   int occupancyThreshold;
   unsigned int clearCurrentRow(double *re_orderedFrame, unsigned int i);
   void clearCurrentRow(uint16_t *re_orderedFrame, unsigned int i);
   int occupancyCorrections;

   /// DEBUGGING:
   int debugFrameCounter;
   std::ofstream outFile;
   void writeFile(const char *buffer, unsigned long length, std::string filePrefix);
};

#endif // GENREALPIXELPROCESSOR_H
