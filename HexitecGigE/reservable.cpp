#include "reservable.h"

Reservable::Reservable()
{
   reserved = false;
   reserver = "";
}

bool Reservable::reserve(QString reserver)
{
   bool success = false;

   if (reserved == false)
   {
      this->reserver = reserver;
      reserved = true;
      success = true;
   }

   return success;
}

bool Reservable::release(QString releaser)
{
   bool success = false;

   if (releaser == reserver)
   {
      reserved = false;
      reserver = "";
      success = true;
   }

   return success;
}

QString Reservable::reservedFor()
{
   return reserver;
}
