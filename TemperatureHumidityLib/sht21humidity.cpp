#include "sht21humidity.h"

SHT21Humidity::SHT21Humidity(QObject *parent):Humidity(parent)
{
   sht21Controller = SHT21Controller::instance(this);
}

SHT21Humidity::~SHT21Humidity()
{
   /*if (sht21Controller != NULL)
   {
      delete sht21Controller;
   }*/
}

double SHT21Humidity::getHumidity()
{
   double humidity = 0.0;
   QMutexLocker locker(&mutex);

   humidity = sht21Controller->ReadI2C(SHT21Controller::HUMIDITY);
   humidity = calcHumidity(humidity);

   return humidity;
}

void SHT21Humidity::close()
{
   if (sht21Controller != NULL)
   {
      sht21Controller->close();
   }
}

double SHT21Humidity::calcHumidity(double humidity)
{
   return (-6 + (125.0 * humidity * 16 / 65536));
}

void SHT21Humidity::destruction()
{
   sht21Controller = NULL;
}
