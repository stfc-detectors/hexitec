#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <QObject>
#include <QMutex>
//#include "reservable.h"

//class Temperature : public Reservable
class Temperature : public QObject
{
   Q_OBJECT
public:
   Temperature(QObject *parent);
   Q_INVOKABLE virtual double getTemperature() = 0;
//   Q_INVOKABLE virtual void set(double temperature) = 0;

protected:
   QMutex mutex;
   double targetTemperature;

private:
   virtual void destruction() = 0;

signals:
   void writeError(QString message);
   void writeMessage(QString message);

private slots:
   void handleWriteError(QString message);
   void handleWriteMessage(QString message);
   void handleDestruction();
};

#endif // TEMPERATURE_H
