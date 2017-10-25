#include "dataacquisitiondefinition.h"
#include "parameters.h"
#include <QSettings>

DataAcquisitionDefinition::DataAcquisitionDefinition()
{
   QSettings settings(QSettings::UserScope, "TEDDI", "HexitecGigE");
   QString twoEasyFilename = Parameters::twoEasyIniFilename;
   double duration = 1000.0;
   int repeatCount = 1;
   int repeatInterval = 0;
   bool offsets = false;
   QString directory = "C:";
   QString prefix = "GigE_";
   QString logDirectory = "C:";
   QString logPrefix = "GigELog_";

   fixedImageCount = 0;

   if (settings.contains("hexitecGigEIniFilename"))
   {
      twoEasyFilename = settings.value("hexitecGigEIniFilename").toString();
   }
   twoEasyIniFile = new IniFile(twoEasyFilename);
   this->duration = duration;
   this->repeatCount = repeatCount;
   this->repeatInterval = repeatCount;
   this->offsets = offsets;
   this->triggering = false;
   this->ttlInput = 0;

   if ((duration = twoEasyIniFile->getDouble("Data_Acquisition/Duration")) != QVariant(INVALID))
   {
      this->duration = duration * 1000.0;
   }
   if ((repeatCount = twoEasyIniFile->getInt("Data_Acquisition/Repeat_Count")) != QVariant(INVALID))
   {
      this->repeatCount = repeatCount;
   }
   if ((repeatInterval = twoEasyIniFile->getInt("Data_Acquisition/Repeat_Interval"))!= QVariant(INVALID))
   {
      this->repeatInterval = repeatInterval;
   }
   if ((offsets = twoEasyIniFile->getBool("Data_Acquisition/Collect_Offsets")) != QVariant(INVALID))
   {
      this->offsets = offsets;
   }
   if ((prefix = twoEasyIniFile->getString("Data_Acquisition/Data_File_Prefix")) != QVariant(INVALID))
   {
      this->dataFilename.setPrefix(prefix);
   }
   if ((directory = twoEasyIniFile->getString("Data_Acquisition/Directory")) != QVariant(INVALID))
   {
      this->dataFilename.setDirectory(directory);
   }
   if ((prefix = twoEasyIniFile->getString("Data_Acquisition/Prefix")) != QVariant(INVALID))
   {
      this->dataFilename.setPrefix(prefix);
   }
   if ((logDirectory = twoEasyIniFile->getString("Data_Acquisition/Log_Directory")) != QVariant(INVALID))
   {
      this->logFilename.setDirectory(logDirectory);
   }
   if ((logPrefix = twoEasyIniFile->getString("Data_Acquisition/Log_Prefix")) != QVariant(INVALID))
   {
      this->logFilename.setPrefix(logPrefix);
   }
}

void DataAcquisitionDefinition::setDataFilename(DetectorFilename dataFilename)
{
   this->dataFilename = dataFilename;
   twoEasyIniFile->setParameter("Data_Acquisition/Prefix", dataFilename.getPrefix());
   twoEasyIniFile->setParameter("Data_Acquisition/Directory", dataFilename.getDirectory());
   twoEasyIniFile->writeIniFile();
}

void DataAcquisitionDefinition::setLogFilename(DetectorFilename logFilename)
{
   this->logFilename = logFilename;
   twoEasyIniFile->setParameter("Data_Acquisition/Log_Prefix", logFilename.getPrefix());
   twoEasyIniFile->setParameter("Data_Acquisition/Log_Directory", logFilename.getDirectory());
   twoEasyIniFile->writeIniFile();
}

void DataAcquisitionDefinition::setOffsets(bool offsets)
{
   this->offsets = offsets;
   twoEasyIniFile->setParameter("Data_Acquisition/Offsets", (QVariant) offsets);
   twoEasyIniFile->writeIniFile();
}

void DataAcquisitionDefinition::setDuration(double duration)
{
   this->duration = duration;
   twoEasyIniFile->setParameter("Data_Acquisition/Duration", (QVariant) (duration / 1000.0));
   twoEasyIniFile->writeIniFile();
}

void DataAcquisitionDefinition::setRepeatCount(int repeatCount)
{
   this->repeatCount = repeatCount;
   twoEasyIniFile->setParameter("Data_Acquisition/Repeat_Count", (QVariant) repeatCount);
   twoEasyIniFile->writeIniFile();
}

void DataAcquisitionDefinition::setRepeatInterval(int repeatInterval)
{
   this->repeatInterval = repeatInterval;
   twoEasyIniFile->setParameter("Data_Acquisition/Repeat_Interval", (QVariant) repeatInterval);
   twoEasyIniFile->writeIniFile();
}

void DataAcquisitionDefinition::setFixedImageCount(int fixedImageCount)
{
   this->fixedImageCount = fixedImageCount;
}

void DataAcquisitionDefinition::setLogging(bool logging)
{
   this->logging = logging;
}

void DataAcquisitionDefinition::setTriggering(bool triggering)
{
   this->triggering = triggering;
}

void DataAcquisitionDefinition::setTtlInput(int ttlInput)
{
   this->ttlInput = ttlInput;
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

bool DataAcquisitionDefinition::isTriggering()
{
   return triggering;
}

int DataAcquisitionDefinition::getTtlInput()
{
   return ttlInput;
}
