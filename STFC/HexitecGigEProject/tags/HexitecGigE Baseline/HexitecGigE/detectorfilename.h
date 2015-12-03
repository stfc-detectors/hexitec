#ifndef DETECTORFILENAME_H
#define DETECTORFILENAME_H

#include <QString>

class DetectorFilename
{
public:
   DetectorFilename();
   DetectorFilename &operator =(DetectorFilename source);
   void setTimestampOn(bool timestampOn);
   void setDirectory(QString directory);
   void setPrefix(QString prefix);
   bool getTimestampOn();
   QString getDirectory();
   QString getPrefix();

private:
   bool timestampOn;
   QString directory;
   QString prefix;
};

#endif // DETECTORFILENAME_H
