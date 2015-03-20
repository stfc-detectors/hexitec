#ifndef SHT21TEMPERATURE_H
#define SHT21TEMPERATURE_H

#include "sht21controller.h"
#include "temperature.h"

class SHT21Temperature : public Temperature
{
public:
   SHT21Temperature(QObject *parent);
   ~SHT21Temperature();
   Q_INVOKABLE double getTemperature();
//   Q_INVOKABLE void set(double temperature);
   void close();

protected:

private:
   SHT21Controller *sht21Controller;
   double calcTemperature(double temperature);
   void destruction();

private slots:

signals:
};

#endif // SHT21TEMPERATURE_H
