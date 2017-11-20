#include "galilmotor.h"

GalilMotor::GalilMotor(QString name, int id, QObject *parent):Motor(name, parent)
{
   motorController = GalilMotorController::instance();
   this->id = id;
   selectString = selector.at(id-1);
   configure();
   axis = axes.at(id-1);
   if (motorController->isActive())
   {
      start();
   }
}

void GalilMotor::run()
{
   while (true)
   {
      msleep(100);

      setPosition(motorController->getPosition(selectString));
      setMotorStatus(MotorStatus(motorController->getStatus(axis)));
   }
}

void GalilMotor::stop()
{
   motorController->stopMotor(axis);
}

void GalilMotor::begin(int desiredPosition)
{
   motorController->setDesiredPosition(selectString, desiredPosition);
   motorController->startMotor(axis);
}

bool GalilMotor::isMoving()
{
   return false;
}

void GalilMotor::configure()
{
   int speed = 5000;

   if (id > 3)
   {
      speed = 2500;
   }
   motorController->configure(selectString, speed);
}
