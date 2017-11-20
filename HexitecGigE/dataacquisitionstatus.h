#ifndef DATAACQUISITIONSTATUS_H
#define DATAACQUISITIONSTATUS_H

#include <QString>

class DataAcquisitionStatus
{
public:
   enum MajorStatus {IDLE,
                     INITIALISING,
                     ACQUIRING_DATA};
   enum MinorStatus {READY,
                     COLLECTING,
                     COLLECTING_PREP,
                     BIAS_REFRESHING,
                     WAITING_TRIGGER,
                     PAUSED,
                     ACTIVE,
                     DONE,
                     OFFSETS,
                     OFFSETS_PREP,
                     WAITING_DARK,
                     TRIGGERING_STOPPED,
                     NOT_INITIALIZED};

   DataAcquisitionStatus(MajorStatus majorStatus = DataAcquisitionStatus::IDLE,
                         MinorStatus minorStatus = DataAcquisitionStatus::READY);
   void setMajorStatus(MajorStatus majorStatus);
   void setMinorStatus(MinorStatus minorStatus);
   void setCurrentImage(int currentImage);
   void setDaqImages(int daqImages);
   void setPercentage(int percentage);
   MajorStatus getMajorStatus();
   MinorStatus getMinorStatus();
   QString getMessage();
   int getCurrentImage();
   int getDaqImages();
   int getPercentage();
private:
   MajorStatus majorStatus;
   MinorStatus minorStatus;
   QString statusMessage;
   int currentImage;
   int daqImages;
   int percentage;
   void constructMessage();
   void constructMajorMessage();
   void constructMinorMessage();
};

#endif // DATAACQUISITIONSTATUS_H
