#include "processingdefinition.h"
//#include "parameters.h"
#include <QSettings>
#include <QDebug>

ProcessingDefinition::ProcessingDefinition()
{

}

void ProcessingDefinition::setThresholdMode(ThresholdMode threshholdMode)
{
   this->threshholdMode = threshholdMode;
}

void ProcessingDefinition::setThresholdValue(int thresholdValue)
{
   this->thresholdValue = thresholdValue;
}

void ProcessingDefinition::setThresholdPerPixel(uint16_t *thresholdPerPixel)
{
   this->thresholdPerPixel = thresholdPerPixel;
}

void ProcessingDefinition::setProcessedFilename(const char *processedFilename)
{
   this->processedFilename = (char *)processedFilename;
}

void ProcessingDefinition::setGradientFilename(const char *gradientFilename)
{
   this->gradientFilename = (char *)gradientFilename;
}

void ProcessingDefinition::setInterceptFilename(const char *interceptFilename)
{
   this->interceptFilename = (char *)interceptFilename;
}

ThresholdMode ProcessingDefinition::getThreshholdMode() const
{
   return threshholdMode;
}

int ProcessingDefinition::getThresholdValue() const
{
   return thresholdValue;
}

uint16_t *ProcessingDefinition::getThresholdPerPixel() const
{
   return thresholdPerPixel;
}

char *ProcessingDefinition::getGradientFilename() const
{
   return gradientFilename;
}

char *ProcessingDefinition::getInterceptFilename() const
{
   return interceptFilename;
}

char *ProcessingDefinition::getProcessedFilename() const
{
   return processedFilename;
}

/*
void ProcessingDefinition::setDataFilename(DetectorFilename dataFilename)
{
   this->dataFilename = dataFilename;
   twoEasyIniFile->setParameter("Data_Acquisition/Prefix", dataFilename.getPrefix());
   twoEasyIniFile->setParameter("Data_Acquisition/Directory", dataFilename.getDirectory());
   twoEasyIniFile->writeIniFile();
}

void ProcessingDefinition::setLogFilename(DetectorFilename logFilename)
{
   this->logFilename = logFilename;
   twoEasyIniFile->setParameter("Data_Acquisition/Log_Prefix", logFilename.getPrefix());
   twoEasyIniFile->setParameter("Data_Acquisition/Log_Directory", logFilename.getDirectory());
   twoEasyIniFile->writeIniFile();
}

void ProcessingDefinition::setOffsets(bool offsets)
{
   this->offsets = offsets;
   twoEasyIniFile->setParameter("Data_Acquisition/Offsets", (QVariant) offsets);
   twoEasyIniFile->writeIniFile();
}

void ProcessingDefinition::setDuration(double duration)
{
   this->duration = duration;
   twoEasyIniFile->setParameter("Data_Acquisition/Duration", (QVariant) (duration / 1000.0));
   twoEasyIniFile->writeIniFile();
}

void ProcessingDefinition::setRepeatCount(int repeatCount)
{
   this->repeatCount = repeatCount;
   twoEasyIniFile->setParameter("Data_Acquisition/Repeat_Count", (QVariant) repeatCount);
   twoEasyIniFile->writeIniFile();
}

void ProcessingDefinition::setRepeatInterval(int repeatInterval)
{
   this->repeatInterval = repeatInterval;
   twoEasyIniFile->setParameter("Data_Acquisition/Repeat_Interval", (QVariant) repeatInterval);
   twoEasyIniFile->writeIniFile();
}

void ProcessingDefinition::setFixedImageCount(int fixedImageCount)
{
   this->fixedImageCount = fixedImageCount;
}

void ProcessingDefinition::setLogging(bool logging)
{
   this->logging = logging;
}

void ProcessingDefinition::setTriggering(bool triggering)
{
   this->triggering = triggering;
}

void ProcessingDefinition::setTtlInput(int ttlInput)
{
   this->ttlInput = ttlInput;
}

DetectorFilename *ProcessingDefinition::getDataFilename()
{
   return &dataFilename;
}

DetectorFilename *ProcessingDefinition::getLogFilename()
{
   return &logFilename;
}

bool ProcessingDefinition::getOffsets()
{
   return offsets;
}

double ProcessingDefinition::getDuration()
{
   return duration;
}

int ProcessingDefinition::getRepeatCount()
{
   return repeatCount;
}

int ProcessingDefinition::getRepeatInterval()
{
   return repeatInterval;
}

int ProcessingDefinition::getFixedImageCount()
{
   return fixedImageCount;
}

bool ProcessingDefinition::isLogging()
{
   return logging;
}

bool ProcessingDefinition::isTriggering()
{
   return triggering;
}

int ProcessingDefinition::getTtlInput()
{
   return ttlInput;
}
*/
