#include "MbusSerialMasterProtocol.hpp"
#include "MbusRtuMasterProtocol.hpp"
#include "west6100plustemperaturecontroller.h"
#include <QDebug>
#include <climits>

West6100PlusTemperatureController *West6100PlusTemperatureController::tcInstance = 0;
MbusRtuMasterProtocol mbusProtocol;

West6100PlusTemperatureController::West6100PlusTemperatureController(char * portName)
{
   this->portName = portName;
   openProtocol();
   temperature = 0.0;
}

West6100PlusTemperatureController *West6100PlusTemperatureController::instance(char *portName)
{
   if (tcInstance == 0)
   {
      tcInstance = new West6100PlusTemperatureController(portName);
   }
   return tcInstance;
}

West6100PlusTemperatureController *West6100PlusTemperatureController::getInstance()
{
   return tcInstance;
}

char *West6100PlusTemperatureController::getSerialNumber()
{
   short readRegSet[5];
   static char serialNumber[13];

   int status;
   status = mbusProtocol.readInputRegisters(1, 123, readRegSet,
                                            sizeof(readRegSet) / sizeof(short));

   sprintf(serialNumber,  "%04x%04x%04x", (unsigned short) readRegSet[1],
         (unsigned short) readRegSet[2],
         (unsigned short) readRegSet[3]);
   serialNumber[12]='\0';
   printf ("West6100PlusTemperatureController::getSerialNumber()Serial Number: %s",serialNumber);
   return serialNumber;
}

double West6100PlusTemperatureController::getTemperature()
{
   short readRegSet[20];
   int status;

   status = mbusProtocol.readInputRegisters(1, 2, readRegSet,
                                            sizeof(readRegSet) / sizeof(short));
   //qDebug() << "West6100PlusTemperatureController::getTemperature() read status " << status;
   if (status != 0)
   {
      reconnect();
      qDebug() << "Reconnected West6100PlusTemperatureController";
      status = mbusProtocol.readInputRegisters(1, 2, readRegSet,
                                               sizeof(readRegSet) / sizeof(short));
      qDebug() << "West6100PlusTemperatureController::getTemperature() status afer reconnect " << status;
      if (status == 0)
      {
         temperature = readRegSet[0]/pow((float)10.0,(float)readRegSet[17]);
      }
   }
   else
   {
      temperature = readRegSet[0]/pow((float)10.0,(float)readRegSet[17]);
   }

   return temperature;
}

void West6100PlusTemperatureController::setTargetTemperature(double targetTemperature)
{
   short decPointPosn;
   short setpoint = 0;
   int status;

   status = mbusProtocol.readInputRegisters(1, 19, &decPointPosn,
                                            sizeof(decPointPosn) / sizeof(short));
   qDebug() << "DecPointPosn" << decPointPosn;
   targetTemperature *= pow((float)10.0,(float) decPointPosn);
   qDebug() << "Target for setting" << targetTemperature;
   if (targetTemperature >= SHRT_MIN && targetTemperature <= SHRT_MAX)
   {
      setpoint = targetTemperature;
      qDebug() << "Target as short for register" << setpoint;
      status = mbusProtocol.writeSingleRegister(1, 3, setpoint);
      qDebug() << "Write status" << status;
   }
   else
   {
      qDebug() << "OUT OF RANGE";
      status = -999;
   }
}

void West6100PlusTemperatureController::openProtocol()
{
   int result;

   result = mbusProtocol.openProtocol(portName,
                                      4800L, // Baudrate
                                      8,      // Databits
                                      1,      // Stopbits
                                      MbusSerialMasterProtocol::SER_PARITY_NONE);     // Even parity
   if (result != FTALK_SUCCESS)
   {
      fprintf(stderr, "Error opening protocol: %s!\n",
              getBusProtocolErrorText(result));
//      exit(EXIT_FAILURE);
   }
}

void West6100PlusTemperatureController::closeProtocol()
{
   mbusProtocol.closeProtocol();
}

void West6100PlusTemperatureController::reconnect()
{
   close();
   Sleep(500);
   openProtocol();
}

void West6100PlusTemperatureController::close()
{
   closeProtocol();
}
