#include "dummymotor.h"

DummyMotor::DummyMotor(QString name, QObject *parent):Motor(name, parent)
{
   configure();
   setPosition(0);
}

void DummyMotor::run()
{
   setMotorStatus(MotorStatus::moving());

   int i = 1;

   if (positionIncrement != 0)
   {

      int newPosition;

      while (i < increments && !stopFlag && limitReachedFlag == 0)
      {
         msleep(sleepTime);
         newPosition = position + positionIncrement;
         if (newPosition > 0 && newPosition >= forwardLimit)
         {
            newPosition = forwardLimit;
            limitReachedFlag = 1;
         }
         else if (newPosition < 0 && newPosition <= reverseLimit)
         {
            newPosition = reverseLimit;
            limitReachedFlag = -1;
         }
         else
         {
            /* Only increment i if not at limit otherwise if you hit the
               limit in the last increment the motor will still move to desiredPosition. */
            i++;
         }
         setPosition(newPosition);
      }
   }

   /* If all the increments have been done, or if positionIncrement was 0 indicating that
      the overall move was too small, then set the position to the desiredPosition (or a
      limit) */
   if (i == increments || positionIncrement == 0)
   {
      msleep(sleepTime);

      if (desiredPosition >= forwardLimit)
      {
         desiredPosition = forwardLimit;
         limitReachedFlag = 1;
      }
      else if (desiredPosition <= reverseLimit)
      {
         desiredPosition = reverseLimit;
         limitReachedFlag = -1;
      }

      setPosition(desiredPosition);
   }

   int newStatus = MotorStatus::LATCHED;
   if (limitReachedFlag == 1)
   {
      newStatus += MotorStatus::FORWARD_LIMIT;
   }
   else if (limitReachedFlag == -1)
   {
      newStatus += MotorStatus::REVERSE_LIMIT;
   }
   setMotorStatus(MotorStatus::MotorStatus(newStatus));
}

void DummyMotor::stop()
{
   QMutexLocker locker(&mutex);
   stopFlag = true;
}

void DummyMotor::begin(int desiredPosition)
{
   positionIncrement = (desiredPosition - position) / increments;
   stopFlag = false;
   limitReachedFlag = 0;
   start();
}

void DummyMotor::configure()
{
   setMotorStatus(MotorStatus::latched());
   increments = 10;
   sleepTime = 500;
   forwardLimit = 200;
   reverseLimit = -200;
}




