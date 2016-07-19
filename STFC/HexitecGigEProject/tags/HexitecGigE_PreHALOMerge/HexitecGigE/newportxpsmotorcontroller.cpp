#include "newportxpsmotorcontroller.h"

NewportXpsMotorController *NewportXpsMotorController::nxmcInstance = 0;

NewportXpsMotorController::NewportXpsMotorController()
{
   port = 5001;
   timeout = 1.0;
//   newportXpsSocketId = TCP_ConnectToServer("192.168.1.70", port, timeout);
   //TCP_SetTimeout(newportXpsSocketId, timeout);
}

NewportXpsMotorController *NewportXpsMotorController::instance()
{
   if (nxmcInstance == 0)
   {
      nxmcInstance = new NewportXpsMotorController();
   }
   return nxmcInstance;
}

void NewportXpsMotorController::configure(int speed)
{
}

void NewportXpsMotorController::setDesiredPosition(int desiredPosition)
{
}

void NewportXpsMotorController::startMotor()
{
}

void NewportXpsMotorController::stopMotor()
{
}

int NewportXpsMotorController::getPosition()
{
   int position = 0;

   return position;
}

int NewportXpsMotorController::getStatus()
{
   int status = 0;

   return status;
}
