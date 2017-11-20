#include "detectorexception.h"

DetectorException::DetectorException(QString message)
{
   this->message = message;
}

DetectorException::DetectorException(string message)
{
   this->message = QString::fromStdString(message);
}
