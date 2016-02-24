#ifndef DUMMYMOTOR_H
#define DUMMYMOTOR_H

#include <string>
#include <QHash>
#include "motor.h"

//enum Status {ERROR = -1, READY = 0, MOVING = 1, LIMIT = 2};

class DummyMotor : public Motor
{
private:
   int increments;
   int sleepTime;
   int positionIncrement;
   int forwardLimit;
   int reverseLimit;
   bool stopFlag;
   int limitReachedFlag;
   void setStatusString(QString);
public:
   DummyMotor(QString name, QObject *parent = 0);
   void run();
   void stop();
   void begin(int desiredPosition);
   void configure();
};

#endif // DUMMYMOTOR_H
