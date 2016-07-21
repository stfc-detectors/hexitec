#ifndef DETECTOREXCEPTION_H
#define DETECTOREXCEPTION_H
#define NOMINMAX

#include <QException>
#include <QString>

using namespace std;

class DetectorException : public QException
{
public:
   DetectorException(string message);
   DetectorException(QString message);
   void raise() const {throw *this;}
   DetectorException *clone() const {return new DetectorException(*this);}
   QString getMessage() const {return message;}

private:
   QString message;
};


#endif // DETECTOREXCEPTION_H
