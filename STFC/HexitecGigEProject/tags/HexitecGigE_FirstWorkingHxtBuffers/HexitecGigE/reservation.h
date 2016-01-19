#ifndef RESERVATION_H
#define RESERVATION_H

#include <QString>
#include <QList>
#include "reservable.h"

class Reservation
{
private:
   QList<Reservable *> reserved;
   QString message;

public:
   Reservation();
   Reservation(QList<Reservable *> reserved, QString message);
   QList<Reservable *> getReserved();
   QString getMessage();
   Reservation add(Reservation toBeAdded);
};

#endif // RESERVATION_H
