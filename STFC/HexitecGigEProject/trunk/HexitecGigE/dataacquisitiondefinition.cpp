#include "dataacquisitiondefinition.h"
#include "parameters.h"
#include <QSettings>
#include <QDebug>

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

   if ((duration = twoEasyIniFile->getDouble("Data Acquisition/Duration")) != QVariant(INVALID))
   {
      this->duration = duration * 1000.0;
   }
   if ((repeatCount = twoEasyIniFile->getInt("Data Acquisition/Repeat Count")) != QVariant(INVALID))
   {
      this->repeatCount = repeatCount;
   }
   if ((repeatInterval = twoEasyIniFile->getInt("Data Acquisition/Repeat Interval"))!= QVariant(INVALID))
   {
      this->repeatInterval = repeatInterval;
   }
   if ((offsets = twoEasyIniFile->getBool("Data Acquisition/Collect Offsets")) != QVariant(INVALID))
   {
      this->offsets = offsets;
   }
   if ((prefix = twoEasyIniFile->getString("Data Acquisition/Data File Prefix")) != QVariant(INVALID))
   {
      this->dataFilename.setPrefix(prefix);
   }
   if ((directory = twoEasyIniFile->getString("Data Acquisition/Directory")) != QVariant(INVALID))
   {
      this->dataFilename.setDirectory(directory);
   }
   if ((prefix = twoEasyIniFile->getString("Data Acquisition/Prefix")) != QVariant(INVALID))
   {
      this->dataFilename.setPrefix(prefix);
   }
   if ((logDirectory = twoEasyIniFile->getString("Data Acquisition/Log Directory")) != QVariant(INVALID))
   {
      this->logFilename.setDirectory(logDirectory);
   }
   if ((logPrefix = twoEasyIniFile->getString("Data Acquisition/Log Prefix")) != QVariant(INVALID))
   {
      this->logFilename.setPrefix(logPrefix);
   }
}

void DataAcquisitionDefinition::setDataFilename(DetectorFilename dataFilename)
{
   twoEasyIniFile->setParameter("Data Acquisition/Prefix", dataFilename.getPrefix());
   twoEasyIniFile->setParameter("Data Acquisition/Directory", dataFilename.getDirectory());
   twoEasyIniFile->writeIniFile();
   this->dataFilename = dataFilename;
}

void DataAcquisitionDefinition::setLogFilename(DetectorFilename logFilename)
{
   twoEasyIniFile->setParameter("Data Acquisition/Log Prefix", logFilename.getPrefix());
   twoEasyIniFile->setParameter("Data Acquisition/Log Directory", logFilename.getDirectory());
   twoEasyIniFile->writeIniFile();
   this->logFilename = logFilename;
}

void DataAcquisitionDefinition::setOffsets(bool offsets)
{
   twoEasyIniFile->setParameter("Data Acquisition/Offsets", (QVariant) offsets);
   twoEasyIniFile->writeIniFile();
   this->offsets = offsets;
}

void DataAcquisitionDefinition::setDuration(double duration)
{
   twoEasyIniFile->setParameter("Data Acquisition/Duration", (QVariant) (duration / 1000.0));
   twoEasyIniFile->writeIniFile();
   this->duration = duration;
}

void DataAcquisitionDefinition::setRepeatCount(int repeatCount)
{
   twoEasyIniFile->setParameter("Data Acquisition/Repeat Count", (QVariant) repeatCount);
   twoEasyIniFile->writeIniFile();
   this->repeatCount = repeatCount;
}

void DataAcquisitionDefinition::setRepeatInterval(int repeatInterval)
{
   twoEasyIniFile->setParameter("Data Acquisition/Repeat Interval", (QVariant) repeatInterval);
   twoEasyIniFile->writeIniFile();
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
