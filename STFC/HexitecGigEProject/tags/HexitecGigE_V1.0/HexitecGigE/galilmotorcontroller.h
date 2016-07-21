#ifndef GALILMOTORCONTROLLER_H
#define GALILMOTORCONTROLLER_H

/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX

#include <QObject>
#include <QMutex>
#include <sstream>
#include <string>
#include <stdio.h>
#include <bitset>
#include <windows.h>
#include <Galil.h>
#include "applicationoutput.h"

using namespace std;

//enum statusCodes {MOTOR_HOME = 2, MOTOR_REVERSE_LIMIT = 4, MOTOR_FORWARD_LIMIT = 8,
//                  MOTOR_ON = 32, MOTOR_ERROR = 64, MOTOR_MOVING = 128};

class GalilMotorController : public QObject
{
   Q_OBJECT

public:
   static GalilMotorController *instance();
   void configure(string selectString, int speed);
   void sendCommand(string command);
   void startMotor(string axis);
   void stopMotor(string axis);
   int getPosition(string selectString);
   int getStatus(string selectString);
   void setDesiredPosition(string selectString, int desiredPosition);
   bool isActive();

private:
   GalilMotorController();
   static GalilMotorController *gmcInstance;
   string intToString(int i);
   Galil *galil;
   QMutex  mutex;
   string command;
   string returnBuffer;
   int position;
   int returnCode;
   int switchStatus;
   int limitMode;
   static const int limitBitMask = 014;
   bool active;

signals:
   void writeError(QString message);
};


#endif // GALILMOTORCONTROLLER_H
