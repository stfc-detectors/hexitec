#ifndef PROCESSINGDEFINITION_H
#define PROCESSINGDEFINITION_H

//#include "inifile.h"
#include "hxtitem.h"
#include <cstdint>
#include <string>

using namespace std;

enum ThresholdMode {NONE, SINGLE_VALUE, THRESHOLD_FILE};
enum ChargedSharingMode {OFF, ADDITION, DISCRIMINATION};

class ProcessingDefinition
{
public:
   ProcessingDefinition(long long frameSize);
   ~ProcessingDefinition();
   void setThresholdMode(ThresholdMode threshholdMode);
   void setThresholdValue(int thresholdValue);
   void setThresholdPerPixel(char *thresholdFilename);
   void setEnergyCalibration(bool energyCalibration);
   void setProcessedFilename();
   void setGradientFilename(char *gradientFilename);
   void setInterceptFilename(char *interceptFilename);
   void setOutputDirectory(char *outputDirectory);
   void setOutputPrefix(char *outputPrefix);

   ThresholdMode getThreshholdMode() const;
   int getThresholdValue() const;
   uint16_t *getThresholdPerPixel() const;
   char *getGradientFilename();
   char *getInterceptFilename();
   char *getProcessedFilename() const;
   double *getGradients();
   double *getIntercepts();

   bool getRe_order() const;
   void setRe_order(bool re_order);

   bool getEnergyCalibration() const;

   long long getBinStart() const;
   void setBinStart(const long long &binStart);

   long long getBinEnd() const;
   void setBinEnd(const long long &binEnd);

   double getBinWidth() const;
   void setBinWidth(const double &binWidth);

   bool getTotalSpectrum() const;
   void setTotalSpectrum(bool totalSpectrum);

   char * getOutputDirectory();
   char * getOutputPrefix();

   long long getFrameSize() const;
   void setFrameSize(long long value);

   long long getHxtBufferAllDataSize() const;

   long long getHxtBufferHeaderSize() const;
   void setHxtBufferHeaderSize(long long value);

   ChargedSharingMode getChargedSharingMode() const;
   void setChargedSharingMode(const ChargedSharingMode &value);

   bool getNextFrameCorrection() const;
   void setNextFrameCorrection(bool value);

   int getPixelGridSize() const;
   void setPixelGridSize(int value);

private:
   void setGradients();
   void setIntercepts();
   void getData(char *filename, double *dataValue);
   void getData(const char *filename, uint16_t *dataValue);
   bool re_order;
   ThresholdMode threshholdMode;
   int thresholdValue;
   uint16_t *thresholdPerPixel;
   bool energyCalibration;
   bool nextFrameCorrection;
   ChargedSharingMode chargedSharingMode;
   int chargedSharingPixels;
   int pixelGridSize;
   long long frameSize;
   long long binStart;
   long long binEnd;
   double binWidth;
   bool totalSpectrum;
   long long hxtBufferAllDataSize;
   long long hxtBufferHeaderSize;
   char *gradientFilename;
   char *interceptFilename;
   char *processedFilename;
   double *gradientValue;
   double *interceptValue;
   char *outputDirectory;
   char *outputPrefix;
   char *outputFilename;

};

#endif // PROCESSINGDEFINITION_H
