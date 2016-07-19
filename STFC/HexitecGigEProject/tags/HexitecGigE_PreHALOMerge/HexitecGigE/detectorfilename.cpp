#include "detectorfilename.h"

DetectorFilename::DetectorFilename()
{
   timestampOn = false;
   directory = "";
   prefix = "";
}

DetectorFilename &DetectorFilename::operator =(DetectorFilename source)
{
   this->timestampOn = source.timestampOn;
   this->directory = QString(source.directory);
   this->prefix = QString(source.prefix);

   return *this;
}

void DetectorFilename::setTimestampOn(bool timestampOn)
{
   this->timestampOn = timestampOn;
}

void DetectorFilename::setDirectory(QString directory)
{
   this->directory = directory;
}

void DetectorFilename::setPrefix(QString prefix)
{
   this->prefix = prefix;
}

bool DetectorFilename::getTimestampOn()
{
   return timestampOn;
}

QString DetectorFilename::getDirectory()
{
   return directory;
}

QString DetectorFilename::getPrefix()
{
   return prefix;
}
