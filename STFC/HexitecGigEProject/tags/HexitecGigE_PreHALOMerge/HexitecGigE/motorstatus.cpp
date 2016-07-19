#include "motorstatus.h"
#include <QMetaType>

int MotorStatus::metaDataType = 0;


MotorStatus::MotorStatus(int status)
{
   /* Avoid ERROR as a string status! */
   stringStatusList << "LATCHED" << "HOME" << "REVERSE_LIMIT" << "FORWARD_LIMIT" << "UNDEFINED" << "OFF" << "ERROR_LIMIT" << "MOVING" << "NOT_INITIALISED" << "HOMING";

   this->status = status;
   stringStatus = statusToString(status);
   /* The following is necessary to allow MotorStatus to be passed through signal/slot calls,
      via the queining connect mechanism. The default mechanism is auto connections. Within the
      same thread this equates to direct but across threads this equates to queued.
      The static variable metaDataType ensures this only gets called once for all MotorStatus classes.
   */
   if (metaDataType == 0)
   {
      metaDataType = qRegisterMetaType<MotorStatus>("MotorStatus");
   }
}

int MotorStatus::getStatus()
{
   return status;
}

QString MotorStatus::getStringStatus()
{
   return stringStatus;
}

/* This method is checking the status string but maybe it should check the bits in
   the status integer.*/
bool MotorStatus::contains(MotorStatus other)
{
   bool contains = false;
   if (getStringStatus().contains(other.getStringStatus()))
   {
      contains = true;
   }

   return contains;
}

bool MotorStatus::equals(MotorStatus other)
{
   bool areEqual = false;
   if (getStatus() == other.getStatus())
   {
      areEqual = true;
   }
   return areEqual;
}

MotorStatus MotorStatus::latched()
{
   return MotorStatus(MotorStatus::LATCHED);
}

MotorStatus MotorStatus::home()
{
   return MotorStatus(MotorStatus::HOME);
}

MotorStatus MotorStatus::reverseLimit()
{
   return MotorStatus(MotorStatus::REVERSE_LIMIT);
}

MotorStatus MotorStatus::forwardLimit()
{
   return MotorStatus(MotorStatus::FORWARD_LIMIT);
}

MotorStatus MotorStatus::undefined()
{
   return MotorStatus(MotorStatus::UNDEFINED);
}

MotorStatus MotorStatus::off()
{
   return MotorStatus(MotorStatus::OFF);
}

MotorStatus MotorStatus::errorLimit()
{
   return MotorStatus(MotorStatus::ERROR_LIMIT);
}

MotorStatus MotorStatus::moving()
{
   return MotorStatus(MotorStatus::MOVING);
}

MotorStatus MotorStatus::notInitialised()
{
   return MotorStatus(MotorStatus::NOT_INITIALISED);
}

MotorStatus MotorStatus::homing()
{
   return MotorStatus(MotorStatus::HOMING);
}

bool MotorStatus::isReady()
{
   return ((status & isReadyMask) == 1);
}

bool MotorStatus::isNotInitialised()
{
   return (status & MotorStatus::NOT_INITIALISED);
}

QString MotorStatus::statusToString(int status)
{
   std::bitset<16> bitStatus(status);
   QString statusString = "";
   bool first = true;

   if (isReady())
   {
      statusString = "READY";
      first = false;
   }

   for (int i = 1; i < stringStatusList.length(); i++)
   {
      if (bitStatus[i])
      {
         if (first)
         {
            first = false;
         }
         else
         {
            statusString += " && ";
         }
         statusString += stringStatusList[i];
      }
   }

   return statusString;
}
