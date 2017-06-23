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
   void setProcessedFilename(const char *processedFilename);
   void setGradientFilename(const char *gradientFilename);
   void setInterceptFilename(const char *interceptFilename);
   /*
   void setDataFilename(DetectorFilename dataFilename);
   void setLogFilename(DetectorFilename logFilename);
   void setOffsets(bool offsets);
   void setDuration(double duration);
   void setRepeatCount(int repeatCount);
   void setRepeatInterval(int repeatInterval);
   void setFixedImageCount(int fixedImageCount);
   void setLogging(bool logging);
   void setTriggering(bool triggering);
   void setTtlInput(int ttlInput);
   DetectorFilename *getDataFilename();
   DetectorFilename *getLogFilename();
   bool getOffsets();
   double getDuration();
   int getRepeatCount();
   int getRepeatInterval();
   int getFixedImageCount();
   bool isLogging();
   bool isTriggering();
   int getTtlInput();
*/
   ThresholdMode getThreshholdMode() const;
   int getThresholdValue() const;
   uint16_t *getThresholdPerPixel() const;
   char *getGradientFilename() const;
   char *getInterceptFilename() const;
   char *getProcessedFilename() const;

private:
   ThresholdMode threshholdMode;
   int thresholdValue;
   uint16_t *thresholdPerPixel;
   char *gradientFilename;
   char *interceptFilename;
   char *processedFilename;
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
