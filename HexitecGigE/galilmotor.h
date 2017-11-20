#ifndef GALILMOTOR_H
#define GALILMOTOR_H

#include <QList>
#include <string>
#include "galilmotorcontroller.h"
#include "motor.h"

using namespace std;

const QList<string> selector = QList<string>() << "" << "," << ",," << ",,," << ",,,," << ",,,,," << ",,,,,," << ",,,,,,,";
const QList<string> axes = QList<string>() << "A" << "B" << "C" << "D" << "E" << "F" << "G" <<"H";

class GalilMotor : public Motor
{
public:
   GalilMotor(QString name, int id, QObject *parent = 0);
   void run();
   void stop();
   void begin(int desiredPosition);
   void configure();

protected:
   char accessString;
   char *reply;
   int error;

private:
   void updateStatus();
   bool isMoving();
   GalilMotorController *motorController;
   string returnBuffer;
   string selectString;
   string axis;
   int id;
   int lastStatus;

};

#endif // GALILMOTOR_H
