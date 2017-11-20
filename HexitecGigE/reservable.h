#ifndef RESERVABLE_H
#define RESERVABLE_H

#include <QtScript>
#include <QString>

class Reservable
{
private:
   bool reserved;
   QString reserver;

public:
   Reservable();
   bool reserve(QString reserver);
   virtual bool release(QString releaser);
   QString reservedFor();
};

#endif // RESERVABLE_H
