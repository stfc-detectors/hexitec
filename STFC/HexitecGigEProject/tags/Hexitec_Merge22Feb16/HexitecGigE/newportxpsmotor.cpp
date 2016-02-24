#include "newportxpsmotor.h"
#include "dataacquisition.h"
#include <QSettings>

NewportXpsMotor::NewportXpsMotor(QString name, int id, QObject *parent):Motor(name, parent)
{
   QSettings *settings = new QSettings(QSettings::UserScope, "TEDDI", "HexitecGigE");
   char pIPAddress[13] = {"192.168.1.70"};
   int port = 5001;
   double timeout = 0.1;
   int status;

   if (settings->contains("newportXpsIPaddress"))
   {
      sprintf_s(pIPAddress, "%s", settings->value("newportXpsIPaddress").toString().toUtf8().data());
   }

   if (settings->contains("newportXpsPort"))
   {
      port = settings->value("newportXpsPort").toInt();
   }

   QString gearingSetting = name+"Gearing";
   if (settings->contains(gearingSetting))
   {
      gearing = settings->value(gearingSetting).toInt();
   }

   motorController = new NewportXpsMotorController(pIPAddress, port, timeout);

   this->id = id;
   axis = (char *) newport_axes.at(id-1).c_str();

   status = motorController->getStatus(axis);

   if (motorController->isActive())
   {
      emit writeMessage("Newport motor " + name + " connected on IP address " + pIPAddress + ":" + QString::number(port));
      start();
   }
}

NewportXpsMotor::~NewportXpsMotor()
{
   if (motorController != NULL)
   {
      terminate();
      delete motorController;
      motorController = NULL;
   }
}

void NewportXpsMotor::run()
{
   double position;

   while (true)
   {
      msleep(100);
      if (motorController != NULL)
      {
         position = motorController->getPosition(axis) * (double) gearing;
         setPosition((int) (position + 0.5));
         setMotorStatus(MotorStatus(interpretStatus(motorController->getStatus(axis))));
      }
   }
}

void NewportXpsMotor::_initialise()
{
   initialiseInProgress = true;
   motorController->initialiseMotor(axis);
}

void NewportXpsMotor::stop()
{
   motorController->stopMotor(axis);
}

void NewportXpsMotor::begin(int desiredPosition)
{
   int errorCode;

   double doubleDesiredPosition = ((double) desiredPosition) / ((double) gearing);
   errorCode = motorController->startMotor(axis, doubleDesiredPosition);
   if (errorCode != 0)
   {
      emit writeError(motorController->getErrorString(errorCode));
   }
}

bool NewportXpsMotor::isMoving()
{
   return false;
}

int NewportXpsMotor::interpretStatus(int newportXpsStatus)
{
   int motorStatus = 16; // UNDEFINED

   if (newportXpsStatus > 9 && newportXpsStatus < 20) // READY
   {
      motorStatus = 1;
   }
   else if (newportXpsStatus == 44)  // MOVING
   {
      motorStatus = 128;
   }
   else if (newportXpsStatus > -1 && newportXpsStatus < 10)  // NOT INITIALISED
   {
      motorStatus = 256;
   }
   else if (newportXpsStatus == 43) // HOMING
   {
      motorStatus = 512;
   }

   return motorStatus;
}

void NewportXpsMotor::configure()
{
}
