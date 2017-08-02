#ifndef PROCESSINGDEFINITION_H
#define PROCESSINGDEFINITION_H

//#include "inifile.h"
#include <cstdint>

using namespace std;

enum ThresholdMode {NONE, SINGLE_VALUE, THRESHOLD_FILE};

class ProcessingDefinition
{
public:
   ProcessingDefinition();

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

private:
   void setGradients();
   void setIntercepts();
   void getData(const char *filename, double *dataValue);
   bool re_order;
   ThresholdMode threshholdMode;
   int thresholdValue;
   uint16_t *thresholdPerPixel;
   bool energyCalibration;
   long long binStart;
   long long binEnd;
   long long binWidth;
   bool totalSpectrum;
   char *gradientFilename;
   char *interceptFilename;
   char *processedFilename;
   double *gradientValue;
   double *interceptValue;

};

#endif // PROCESSINGDEFINITION_H
