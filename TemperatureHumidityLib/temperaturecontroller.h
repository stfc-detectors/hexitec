#ifndef TEMPERATURECONTROLLER_H
#define TEMPERATURECONTROLLER_H

#include <QObject>
#include <QMutex>
#include "temperature.h"
//#include "reservable.h"

//class TemperatureController : public Temperature
class TemperatureController : public Temperature
{
   Q_OBJECT
public:
   TemperatureController(QObject *parent);
   Q_INVOKABLE virtual void setTargetTemperature(double temperature) = 0;
   Q_INVOKABLE virtual double getTemperature() = 0;

protected:
   QMutex mutex;
   double temperature;
   double targetTemperature;

private:

signals:

private slots:
};

#endif // TEMPERATURECONTROLLER_H
