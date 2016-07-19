#include "reservation.h"

Reservation::Reservation()
{
   this->reserved = QList<Reservable *>();
}

Reservation::Reservation(QList<Reservable *> reserved, QString message)
{
   this->reserved = reserved;
   this->message = message;
}

Reservation Reservation::add(Reservation toBeAdded)
{
   QList<Reservable *> reserved = this->reserved + toBeAdded.getReserved();
   QString message = toBeAdded.getMessage();
   return Reservation(reserved, message);
}

QList<Reservable *> Reservation::getReserved()
{
   return reserved;
}

QString Reservation::getMessage()
{
   return message;
}
