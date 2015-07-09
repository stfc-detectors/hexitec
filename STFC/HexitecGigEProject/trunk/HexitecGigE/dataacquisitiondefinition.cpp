#include "dataacquisitiondefinition.h"
#include "parameters.h"
#include <QSettings>

DataAcquisitionDefinition::DataAcquisitionDefinition()
{
   QSettings settings(QSettings::UserScope, "TEDDI", "2Easy");
   QString twoEasyFilename = Parameters::twoEasyIniFilename;

   duration = 1000.0;
   repeatCount = 1;
   repeatInterval = 0;
   fixedImageCount = 0;

   if (settings.contains("2EasyIniFilename"))
   {
      twoEasyFilename = settings.value("2EasyIniFilename").toString();
   }
   twoEasyIniFile = new IniFile(twoEasyFilename);
//   repeatCount = twoEasyIniFile->getInt("Data Acquisition/Repeat Count");
}

void DataAcquisitionDefinition::setDataFilename(DetectorFilename dataFilename)
{
   this->dataFilename = dataFilename;
}

void DataAcquisitionDefinition::setLogFilename(DetectorFilename logFilename)
{
   this->logFilename = logFilename;
}

void DataAcquisitionDefinition::setOffsets(bool offsets)
{
   this->offsets = offsets;
}

void DataAcquisitionDefinition::setDuration(double duration)
{
   this->duration = duration;
}

void DataAcquisitionDefinition::setRepeatCount(int repeatCount)
{
   this->repeatCount = repeatCount;
}

void DataAcquisitionDefinition::setRepeatInterval(int repeatInterval)
{
   this->repeatInterval = repeatInterval;
}

void DataAcquisitionDefinition::setFixedImageCount(int fixedImageCount)
{
   this->fixedImageCount = fixedImageCount;
}

void DataAcquisitionDefinition::setLogging(bool logging)
{
   this->logging = logging;
}

DetectorFilename *DataAcquisitionDefinition::getDataFilename()
{
   return &dataFilename;
}

DetectorFilename *DataAcquisitionDefinition::getLogFilename()
{
   return &logFilename;
}

bool DataAcquisitionDefinition::getOffsets()
{
   return offsets;
}

double DataAcquisitionDefinition::getDuration()
{
   return duration;
}

int DataAcquisitionDefinition::getRepeatCount()
{
   return repeatCount;
}

int DataAcquisitionDefinition::getRepeatInterval()
{
   return repeatInterval;
}

int DataAcquisitionDefinition::getFixedImageCount()
{
   return fixedImageCount;
}

bool DataAcquisitionDefinition::isLogging()
{
   return logging;
}
