#include "sht21controller.h"
#include "iowkit.h"

SHT21Controller *SHT21Controller::sht21cInstance = 0;

SHT21Controller::SHT21Controller()
{
   connected = false;
   ioHandle = NULL;
}

SHT21Controller::~SHT21Controller()
{
   close();
}

SHT21Controller *SHT21Controller::instance(QObject *parent)
{
   if (sht21cInstance == 0)
   {
      sht21cInstance = new SHT21Controller();
   }

   connect(sht21cInstance, SIGNAL(writeError(QString)), parent, SLOT(handleWriteError(QString)));
   connect(sht21cInstance, SIGNAL(writeMessage(QString)), parent, SLOT(handleWriteMessage(QString)));
   connect(sht21cInstance, SIGNAL(destruction()), parent, SLOT(handleDestruction()));
   sht21cInstance->init();
   return sht21cInstance;
}

void SHT21Controller::init()
{
   if (!connected)
   {
      ioHandle = IowKitOpenDevice();

      if (ioHandle != NULL)
      {
         numDevices = IowKitGetNumDevs();
         if (numDevices == 1)
         {
            emit writeMessage("Sensirion opened for one device");
            connected = true;
            IOWKIT_SPECIAL_REPORT report;
            memset(&report, 0x00, IOWKIT_SPECIAL_REPORT_SIZE);

            report.ReportID = 0x01; //I2C-Mode
            report.Bytes[0] = 0x01; //Enable
            report.Bytes[1] = 0xC0; //Disable Pull-Up resistors, Enable Bus

            IowKitWrite(ioHandle, IOW_PIPE_SPECIAL_MODE, (char*) &report, IOWKIT_SPECIAL_REPORT_SIZE);
         }
         else
         {
            emit writeError("Sensirion controller currently only works with a single device");
         }
      }
      else
      {
         connected = false;
         emit writeError("Failed to connect to Sensirion SHT21 USB Controller");
      }
   }
}

void SHT21Controller::close()
{
   emit destruction();
   if (ioHandle != NULL)
   {
      IowKitCloseDevice(ioHandle);
      ioHandle = NULL;
   }
}

short SHT21Controller::ReadI2C(UCHAR call)
{
   unsigned short result = 0;
   IOWKIT_SPECIAL_REPORT report;

   if (connected)
   {
      memset(&report, 0x00, IOWKIT_SPECIAL_REPORT_SIZE);
      report.ReportID = 0x03; //I2C-Read
      report.Bytes[0] = 0x03; //Read 3 Bytes
      report.Bytes[1] = call; //Address for humidity or temperature

      IowKitWrite(ioHandle, IOW_PIPE_SPECIAL_MODE, (char*) &report, IOWKIT_SPECIAL_REPORT_SIZE);
      IowKitRead(ioHandle, IOW_PIPE_SPECIAL_MODE, (char*) &report, IOWKIT_SPECIAL_REPORT_SIZE);

      result = (report.Bytes[1] << 8) | report.Bytes[2];	//create a 16bit value
      result &=~0x03;
   }

   return result;
}
