#ifndef DETECTOREXCEPTION_H
#define DETECTOREXCEPTION_H

#include <QException>
#include <QString>

class DetectorException : public QException
{
private:
   QString message;
public:
   DetectorException(QString message);
   void raise() const {throw *this;}
   DetectorException *clone() const {return new DetectorException(*this);}
   QString getMessage() const {return message;}
};


#endif // DETECTOREXCEPTION_H
