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

   uint16_t getBinStart() const;
   void setBinStart(const uint16_t &binStart);

   uint16_t getBinEnd() const;
   void setBinEnd(const uint16_t &binEnd);

   uint16_t getBinWidth() const;
   void setBinWidth(const uint16_t &binWidth);

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
   uint16_t binStart;
   uint16_t binEnd;
   uint16_t binWidth;
   bool totalSpectrum;
   char *gradientFilename;
   char *interceptFilename;
   char *processedFilename;
   double *gradientValue;
   double *interceptValue;
   /*
   DetectorFilename dataFilename;
   DetectorFilename logFilename;
   IniFile *twoEasyIniFile;
   bool logging;
   bool offsets;
   bool triggering;
   int ttlInput;
   double duration;
   int repeatCount;
   int repeatInterval;
   int fixedImageCount;
   */
};

#endif // PROCESSINGDEFINITION_H
