#ifndef OBJECTRESERVER_H
#define OBJECTRESERVER_H

#include <QMutex>
#include <QObject>
#include <QList>

#include "reservation.h"
#include "reservable.h"

class ObjectReserver : public QObject
{
   Q_OBJECT

private:
   QMutex mutex;
   ObjectReserver(QObject *parent = 0);
   ~ObjectReserver();
   static ObjectReserver *objectReserverInstance;
   QList<Reservable *> reserved;
   bool status;
   bool reserveObjectsFromScript(QString script);
   bool objectInScript(QString objectName, QString script);
   QString reservableName(Reservable *reservable);
   bool isFreeForScript(QObject *object);
   QString daqName;
   QString daqModelName;
   QList<Reservable *> controlledByGuiList;
public:
   static ObjectReserver *instance();
   bool getStatus();
   Reservation reserveForScript(QString script, QList<QObject *> objects, QList<QObject *> freeObjects);
   Reservation reserveForGUI(QList<QObject *> objects);
   void release(QList<Reservable *> reserved, QString reserver, bool scriptIsRunning = true);

signals:
   void scriptReserve(QString name);
   void scriptRelease(QString name);
   void controlledByGui();
};

#endif // OBJECTRESERVER_H
