#include "dataacquisitionstatus.h"

DataAcquisitionStatus::DataAcquisitionStatus(MajorStatus majorStatus, MinorStatus minorStatus)
{
   this->majorStatus = majorStatus;
   this->minorStatus = minorStatus;
   constructMessage();
}

void DataAcquisitionStatus::setMajorStatus(MajorStatus majorStatus)
{
   this->majorStatus = majorStatus;
   constructMessage();
}

void DataAcquisitionStatus::setMinorStatus(MinorStatus minorStatus)
{
   this->minorStatus = minorStatus;
   constructMessage();
}

void DataAcquisitionStatus::setCurrentImage(int currentImage)
{
   this->currentImage = currentImage;
}

void DataAcquisitionStatus::setDaqImages(int daqImages)
{
   this->daqImages = daqImages;
}

void DataAcquisitionStatus::setPercentage(int percentage)
{
   this->percentage = percentage;
}

DataAcquisitionStatus::MajorStatus DataAcquisitionStatus::getMajorStatus()
{
   return majorStatus;
}

DataAcquisitionStatus::MinorStatus DataAcquisitionStatus::getMinorStatus()
{
   return minorStatus;
}

void DataAcquisitionStatus::constructMessage()
{
   constructMajorMessage();
   constructMinorMessage();
}

void DataAcquisitionStatus::constructMajorMessage()
{
   switch (majorStatus)
   {
   case IDLE:
      statusMessage = "Idle";
      break;
   case ACQUIRING_DATA:
      statusMessage = "Acquiring data";
      break;
   case INITIALISING:
      statusMessage = "Initialising";
      break;
   default:
      break;
   }
}

void DataAcquisitionStatus::constructMinorMessage()
{
   switch (minorStatus)
   {
   case READY:
      statusMessage += " : detector ready";
      break;
   case COLLECTING:
      statusMessage += " : collecting image";
      break;
   case BIAS_REFRESHING:
      statusMessage += " : bias refreshing";
      break;
   case PAUSED:
      statusMessage += " : pausing";
      break;
   case ACTIVE:
      statusMessage += " : in progress";
      break;
   case DONE:
      statusMessage += " : complete";
      break;
   case COLLECTING_PREP:
      statusMessage += " : waiting for user";
      break;
   case OFFSETS:
      statusMessage += " : collecting offsets";
      break;
   case OFFSETS_PREP:
      statusMessage += " : waiting for user";
      break;
   case WAITING_DARK:
      statusMessage += " : waiting for user";
      break;
   case WAITING_TRIGGER:
      statusMessage += " : waiting for trigger";
      break;
   case NOT_INITIALIZED:
      statusMessage += " : detector not initialized";
      break;
   default:
      break;
   }
}

QString DataAcquisitionStatus::getMessage()
{
   return statusMessage;
}

int DataAcquisitionStatus::getCurrentImage()
{
   return currentImage;
}

int DataAcquisitionStatus::getDaqImages()
{
   return daqImages;
}

int DataAcquisitionStatus::getPercentage()
{
   return percentage;
}
