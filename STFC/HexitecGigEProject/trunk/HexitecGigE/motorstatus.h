#ifndef MOTORSTATUS_H
#define MOTORSTATUS_H

#include <QString>
#include <QList>
#include <bitset>

class MotorStatus
{
private:
   QList<QString> stringStatusList;
   int status;
   QString stringStatus;
   QString statusToString(int status);
   static int metaDataType;
   static const int isReadyMask = 0361;

public:
   /* If the statusCodes list is modified be SURE to correspondingly modify the initialisation
      of statusStringList in the constructor. */
   enum statusCodes {LATCHED = 1, HOME = 2, REVERSE_LIMIT = 4, FORWARD_LIMIT = 8, UNDEFINED = 16, OFF = 32,
                     ERROR_LIMIT = 64, MOVING = 128, NOT_INITIALISED = 256, HOMING = 512};
   MotorStatus(int status = 0);
   int getStatus();
   QString getStringStatus();
   bool equals(MotorStatus other);
   bool contains(MotorStatus other);
   static MotorStatus latched();
   static MotorStatus home();
   static MotorStatus reverseLimit();
   static MotorStatus forwardLimit();
   static MotorStatus undefined();
   static MotorStatus off();
   static MotorStatus errorLimit();
   static MotorStatus moving();
   static MotorStatus notInitialised();
   static MotorStatus homing();
   bool isReady();
   bool isNotInitialised();
};

#endif // MOTORSTATUS_H
