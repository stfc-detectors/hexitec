#ifndef SHT21HUMIDITY_H
#define SHT21HUMIDITY_H

#include "sht21controller.h"
#include "humidity.h"

class SHT21Humidity : public Humidity
{
public:
   SHT21Humidity(QObject *parent);
   ~SHT21Humidity();
   Q_INVOKABLE double getHumidity();
   void close();

protected:

private:
   SHT21Controller *sht21Controller;
   double calcHumidity(double humidity);
   void destruction();

private slots:

signals:
};

#endif // SHT21HUMIDITY_H
