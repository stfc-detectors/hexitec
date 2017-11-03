#ifndef PROCESSINGDEFINITION_H
#define PROCESSINGDEFINITION_H

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
   bool setThresholdPerPixel(char *thresholdFilename);
   void setEnergyCalibration(bool energyCalibration);
   void setHxtGeneration(bool hxtGeneration);
   void setProcessedFilename();
   bool setGradientFilename(char *gradientFilename);
   bool setInterceptFilename(char *interceptFilename);
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
   bool getHxtGeneration() const;
   long long getBinStart() const;
   void setBinStart(const long long &binStart);
   long long getBinEnd() const;
   void setBinEnd(const long long &binEnd);
   double getBinWidth() const;
   void setBinWidth(const double &binWidth);
   long long getNBins();
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
   int getRows();
   void setRows(int nRows);
   int getCols();
   void setCols(int nCols);
   bool getThresholdsStatus();
   bool getGradientsStatus();
   bool getInterceptsStatus();

private:
   void setGradients();
   void setIntercepts();
   bool getData(char *filename, double *dataValue, double defaultValue);
   bool getData(const char *filename, uint16_t *dataValue, uint16_t defaultValue);
   bool re_order;
   ThresholdMode threshholdMode;
   int thresholdValue;
   uint16_t *thresholdPerPixel;
   bool energyCalibration;
   bool hxtGeneration;
   bool nextFrameCorrection;
   ChargedSharingMode chargedSharingMode;
   int chargedSharingPixels;
   int pixelGridSize;
   long long frameSize;
   int nRows;
   int nCols;
   long long binStart;
   long long binEnd;
   double binWidth;
   long long nBins;
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
   bool thresholdsStatus;
   bool gradientsStatus;
   bool interceptsStatus;

};

#endif // PROCESSINGDEFINITION_H
