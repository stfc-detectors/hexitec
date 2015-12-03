#include "galilmotorcontroller.h"

GalilMotorController *GalilMotorController::gmcInstance = 0;

GalilMotorController::GalilMotorController()
{
   connect(this, SIGNAL(writeError(QString)), ApplicationOutput::instance(), SLOT(writeError(QString)));

   active = false;
   try
   {
      galil = new Galil("192.168.1.2");
      active = true;
   }
   catch (string e)
   {
      emit writeError("Failed to connect to Galil Controller");
      emit writeError(QString(e.c_str()));
   }

   limitMode = 1;
}

GalilMotorController *GalilMotorController::instance()
{
   if (gmcInstance == 0)
   {
      gmcInstance = new GalilMotorController();
   }
   return gmcInstance;
}

void GalilMotorController::configure(string selectString, int speed)
{
   mutex.lock();
   // set motor to stepper motor with reversed direction and active low pulses
   command = "MT "  + selectString + "2.5";
   sendCommand(command);
   // set acceleration smoothing values
   command = "KS "  + selectString + intToString(15);
   sendCommand(command);
   // set Speed
   command = "SP "  + selectString + intToString(speed);
   sendCommand(command);
   // set configuration of the switches but see comment in getMotorStatus
   command = "CN " + intToString(limitMode);
   sendCommand(command);
   mutex.unlock();
}

int GalilMotorController::getPosition(string selectString)
{
   int position = 0;

   mutex.lock();
   command = "PA " + selectString + "?";
   sendCommand(command);
   if (!returnBuffer.empty())
   {
      position = atoi(returnBuffer.c_str());
   }
   mutex.unlock();

   return position;
}

int GalilMotorController::getStatus(string axis)
{
   int status = 16;
   mutex.lock();
   command = "TS " + axis;
   sendCommand(command);
   if (!returnBuffer.empty())
   {
      status = atoi(returnBuffer.c_str());
      /* The Galil status FORWARD and REVERSE LIMIT bits are 1 when the motor is
      NOT at the limit so we need to flip those bits to match our motor status
      which has these bits set when you ARE at a limit. */
      status = status ^ limitBitMask;
   }
   mutex.unlock();
   return status;
}

void GalilMotorController::setDesiredPosition(string selectString, int desiredPosition)
{
   mutex.lock();
   command = "PA " + selectString + intToString(desiredPosition);
   sendCommand(command);
   mutex.unlock();
}

bool GalilMotorController::isActive()
{
   return active;
}

void GalilMotorController::startMotor(string axis)
{
   mutex.lock();
   command = "BG " + axis;
   sendCommand(command);
   mutex.unlock();
}

void GalilMotorController::stopMotor(string axis)
{
   mutex.lock();
   command = "ST" + axis;
   sendCommand(command);
   mutex.unlock();
}

void GalilMotorController::sendCommand(string command)
{
   returnBuffer.clear();
   if (active)
   {
      try
      {
         returnBuffer = galil->command(command);
      }
      catch (string e)
      {
         emit writeError(QString(e.c_str()));
      }
   }
   else
   {
      emit writeError("Galil command failed - no connection to Galil Controller");
   }
}

string GalilMotorController::intToString(int i)
{
   stringstream st;
   st << i;
   return st.str();
}
