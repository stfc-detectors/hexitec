#ifndef DATAACQUISITIONDEFINITION_H
#define DATAACQUISITIONDEFINITION_H

#include "detectorfilename.h"
#include "inifile.h"

class DataAcquisitionDefinition
{
public:
   DataAcquisitionDefinition();
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

private:
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
};

#endif // DATAACQUISITIONDEFINITION_H
