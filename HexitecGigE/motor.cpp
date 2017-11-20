#include "motor.h"

Motor::Motor(QString name, QObject *parent) :
   QThread(parent)
{
   connect(this, SIGNAL(writeError(QString)), ApplicationOutput::instance(), SLOT(writeError(QString)));
   connect(this, SIGNAL(writeMessage(QString)), ApplicationOutput::instance(), SLOT(writeMessage(QString)));

   motorReleaser = "GUIReserver";
   moveInProgress = false;
   initialiseInProgress = false;
   setPosition(0);
   setMotorStatus(MotorStatus::undefined());
   setMoveInput(0, false);
   setMode(ABSOLUTE_MODE, false);
   gearing = 1;
   this->name = name;
}

void Motor::initialise()
{
   QMutexLocker locker(&mutex);

   _initialise();
}

void Motor::_initialise()
{
}

void Motor::reset()
{
}

int Motor::getPosition()
{
   QMutexLocker locker(&mutex);
   return position;
}

void Motor::setPosition(int position)
{
   QMutexLocker locker(&mutex);

   if (this->position != position)
   {
      this->position = position;
      emit positionChanged(this, QVariant(position));
   }
}

QString Motor::getStatusString()
{
   return getMotorStatus().getStringStatus();
}

void Motor::setMotorStatus(MotorStatus motorStatus)
{
   QMutexLocker locker(&mutex);

   if (!this->motorStatus.equals(motorStatus))
   {
      this->motorStatus = motorStatus;
      emit statusChanged(this, motorStatus);
   }

   if (moveInProgress && motorStatus.isReady())
   {
      moveInProgress = false;
      trySelfRelease();
   }
   else if (initialiseInProgress && motorStatus.isReady())
   {
      initialiseInProgress = false;
      trySelfRelease();
   }
}

MotorStatus Motor::getMotorStatus()
{
   QMutexLocker locker(&mutex);
   return motorStatus;
}

/* The doEmit variable indicates whether or not a signal should be emitted. Views are
   connected to this signal. */
void Motor::setMode(int mode, bool doEmit)
{
   QMutexLocker locker(&mutex);
   this->mode = mode;
   if (doEmit)
   {
      modeChanged(this, QVariant(mode));
   }
}

int Motor::getMode()
{
   QMutexLocker locker(&mutex);
   return mode;
}

void Motor::setSelected(bool selected)
{
   QMutexLocker locker(&mutex);
   this->selected = selected;
}

bool Motor::isSelected()
{
   QMutexLocker locker(&mutex);
   return selected;
}

/* The doEmit variable indicates whether or not a signal should be emitted. Views are
   connected to this signal. */
void Motor::setMoveInput(int moveInput, bool doEmit)
{
   QMutexLocker locker(&mutex);
   this->moveInput = moveInput;
   if (doEmit)
   {
      emit moveInputChanged(this, QVariant(moveInput));
   }
}

int Motor::getMoveInput()
{
   QMutexLocker locker(&mutex);
   return moveInput;
}

void Motor::setDesiredMove(int moveInput, int mode)
{
   setMoveInput(moveInput, true);
   setMode(mode, true);
}

void Motor::calculateDesiredPosition()
{
   // This method does not need locking since it is called only from begin()
   switch (mode)
   {
   case ABSOLUTE_MODE:
      desiredPosition = moveInput;
      break;
   case RELATIVE_MODE:
      desiredPosition = position + moveInput;
      break;
   }
}

void Motor::begin()
{
   /* We want the whole begin() operation to be atomic which means we must get
      the lock here and leave it in place until the move has started. This in
      turn means we have to access motorStatus directly rather than use
      getMotorStatus() which has its own lock. */
   QMutexLocker locker(&mutex);
   if (willGo(false))
   {
      calculateDesiredPosition();
      // The moveInProgress flag is needed to make releasing work in small moves.
      // It is possible for a move to take less than the polling interval and so
      // status is always seen as READY.
      moveInProgress = true;
      begin(desiredPosition);
   }
}

bool Motor::drivingIntoLimit()
{
   bool drivingIntoLimit = false;
   int direction = 1;

   switch (mode)
   {
   case ABSOLUTE_MODE:
      if (moveInput < position)
      {
         direction = -1;
      }
      break;
   case RELATIVE_MODE:
      if (moveInput < 0)
      {
         direction = -1;
      }
      break;
   }

   if (motorStatus.contains(MotorStatus::reverseLimit()) && direction == -1)
   {
      drivingIntoLimit = true;
   }
   else if (motorStatus.contains(MotorStatus::forwardLimit()) && direction == 1)
   {
      drivingIntoLimit = true;
   }

   return drivingIntoLimit;
}

/* The motor will drive if it is not moving and the move set is not driving it into an already
   actived limit. */
bool Motor::willGo(bool doLock)
{   
   if (doLock)
   {
      QMutexLocker locker(&mutex);
   }

   return motorStatus.isReady() && !drivingIntoLimit();
}

bool Motor::willInitialise(bool doLock)
{
   if (doLock)
   {
      QMutexLocker locker(&mutex);
   }

   return motorStatus.isNotInitialised();
}

/* This method releases the motor using the current value of motorReleaser and restores
   motorReleaser to its default value. */
void Motor::trySelfRelease()
{
   bool released = Reservable::release(motorReleaser);

   if (motorReleaser == "ScriptReserver" && !released)
   {
      emit writeError("Failed to release motor ");
   }

   motorReleaser = "GUIReserver";
}

/* This method overrides Reservable::release. It exists to prevent motors from being released
   when they are still moving. This is most likely to happen when a script has started a long
   move and then ended without waiting for the move to finish. The releaser, which must be the
   current reserver, is remembered and the motor will release itself when it stops moving. */
bool Motor::release(QString releaser)
{
   bool success = false;

   if (releaser == reservedFor())
   {
      // This method must assume that the eventual release will work and return true
      //if(motorStatus.contains(MotorStatus::moving()))
      if (moveInProgress)
      {
         motorReleaser = releaser;
         success = true;
      }
      // Otherwise call the base class method and return its return value
      else
      {
         success = Reservable::release(releaser);
      }
   }
   return success;
}

bool Motor::isMoving()
{
   return moveInProgress;
}
