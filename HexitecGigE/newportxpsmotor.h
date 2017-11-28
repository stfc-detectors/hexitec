#ifndef NEWPORTXPSMOTOR_H
#define NEWPORTXPSMOTOR_H

#include <QThread>
#include <string>
//#include "newportxpsmotorcontroller.h"
#include "motor.h"

using namespace std;

const QList<string> newport_axes = QList<string>() << "Group1" << "Group2" << "Group3" << "Group4" << "Group5" << "Group6" << "Group7" <<"Group8";

class NewportXpsMotor : public Motor
{
public:
   NewportXpsMotor(QString name, int id, QObject *parent = 0);
   ~NewportXpsMotor();
   void run();
   void stop();
   void begin(int desiredPosition);
   void configure();
   void _initialise();

protected:

private:
   void updateStatus();
   bool isMoving();
   int interpretStatus(int newportXpsStatus);
//   NewportXpsMotorController *motorController;
   int id;
   char *axis;
};

#endif // NEWPORTXPSMOTOR_H
