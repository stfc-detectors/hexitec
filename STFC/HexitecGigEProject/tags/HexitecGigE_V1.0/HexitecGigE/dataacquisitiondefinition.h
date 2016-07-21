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
   DetectorFilename *getDataFilename();
   DetectorFilename *getLogFilename();
   bool getOffsets();
   double getDuration();
   int getRepeatCount();
   int getRepeatInterval();
   int getFixedImageCount();
   bool isLogging();

private:
   DetectorFilename dataFilename;
   DetectorFilename logFilename;
   IniFile *twoEasyIniFile;
   bool logging;
   bool offsets;
   double duration;
   int repeatCount;
   int repeatInterval;
   int fixedImageCount;
};

#endif // DATAACQUISITIONDEFINITION_H
