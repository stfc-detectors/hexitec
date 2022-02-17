#include "sht21temperature.h"

SHT21Temperature::SHT21Temperature(QObject *parent):Temperature(parent)
{
   sht21Controller = SHT21Controller::instance(this);
}

SHT21Temperature::~SHT21Temperature()
{
   /*if (sht21Controller != NULL)
   {
      delete sht21Controller;
   }*/
}

double SHT21Temperature::getTemperature()
{
   double temperature = 0.0;
   QMutexLocker locker(&mutex);

   temperature = sht21Controller->ReadI2C(SHT21Controller::TEMPERATURE);
   temperature = calcTemperature(temperature);

   return temperature;
}

/*void SHT21Temperature::set(double temperature)
{
   QMutexLocker locker(&mutex);
   setTemperature = temperature;
}*/

void SHT21Temperature::close()
{
   if (sht21Controller != NULL)
   {
      sht21Controller->close();
   }
}

double SHT21Temperature::calcTemperature(double temperature)
{
   return (-46.85 + ((175.72/65536) * 4 * temperature));
}

void SHT21Temperature::destruction()
{
   sht21Controller = NULL;
}
