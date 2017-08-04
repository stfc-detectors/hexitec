#ifndef PROCESSINGDEFINITION_H
#define PROCESSINGDEFINITION_H

//#include "inifile.h"
#include "hxtitem.h"
#include <cstdint>
#include <string>

using namespace std;

enum ThresholdMode {NONE, SINGLE_VALUE, THRESHOLD_FILE};

class ProcessingDefinition
{
public:
   ProcessingDefinition(long long frameSize);

   void setThresholdMode(ThresholdMode threshholdMode);
   void setThresholdValue(int thresholdValue);
   void setThresholdPerPixel(uint16_t *thresholdPerPixel);
   void setEnergyCalibration(bool energyCalibration);
   void setProcessedFilename(const char *processedFilename);
   void setGradientFilename(const char *gradientFilename);
   void setInterceptFilename(const char *interceptFilename);

   ThresholdMode getThreshholdMode() const;
   int getThresholdValue() const;
   uint16_t *getThresholdPerPixel() const;
   char *getGradientFilename() const;
   char *getInterceptFilename() const;
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

   long long getBinWidth() const;
   void setBinWidth(const long long &binWidth);

   bool getTotalSpectrum() const;
   void setTotalSpectrum(bool totalSpectrum);

   string getOutputDirectory() const;
   void setOutputDirectory(const string &value);

   string getOutputPrefix() const;
   void setOutputPrefix(const string &value);

   long long getFrameSize() const;
   void setFrameSize(long long value);

   long long getHxtBufferAllDataSize() const;

   long long getHxtBufferHeaderSize() const;
   void setHxtBufferHeaderSize(long long value);

private:
   void setGradients();
   void setIntercepts();
   void getData(const char *filename, double *dataValue);
   bool re_order;
   ThresholdMode threshholdMode;
   int thresholdValue;
   uint16_t *thresholdPerPixel;
   bool energyCalibration;
   long long frameSize;
   long long binStart;
   long long binEnd;
   long long binWidth;
   bool totalSpectrum;
   long long hxtBufferAllDataSize;
   long long hxtBufferHeaderSize;
   char *gradientFilename;
   char *interceptFilename;
   char *processedFilename;
   double *gradientValue;
   double *interceptValue;
   string outputDirectory;
   string outputPrefix;

};

#endif // PROCESSINGDEFINITION_H
