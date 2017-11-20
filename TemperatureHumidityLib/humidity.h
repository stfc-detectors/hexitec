#ifndef HUMIDITY_H
#define HUMIDITY_H

#include <QObject>
#include <QMutex>
//#include "reservable.h"

//class Humidity : public Reservable
class Humidity : public QObject
{
   Q_OBJECT
public:
   Humidity(QObject *parent);
   Q_INVOKABLE virtual double getHumidity() = 0;

protected:
   QMutex mutex;

private:
   virtual void destruction() = 0;

signals:
   void writeError(QString message);

private slots:
   void handleWriteError(QString message);
   void handleWriteMessage(QString message);
   void handleDestruction();
};

#endif // HUMIDITY_H
