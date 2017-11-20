#ifndef WEST6100PLUSTEMPERATURECONTROLLER_H
#define WEST6100PLUSTEMPERATURECONTROLLER_H

#include <stdio.h>
#include <math.h>

class West6100PlusTemperatureController
{
public:
   static West6100PlusTemperatureController *instance(char *portName);
   static West6100PlusTemperatureController *getInstance();
   double getTemperature();
   char *getSerialNumber();
   void setTargetTemperature(double targetTemperature);
   void close();
private:
   West6100PlusTemperatureController(char *portName);
   static West6100PlusTemperatureController *tcInstance;
   void openProtocol();
   void closeProtocol();
   void reconnect();
   char *portName;
   double temperature;
};

#endif // WEST6100PLUSTEMPERATURECONTROLLER_H
