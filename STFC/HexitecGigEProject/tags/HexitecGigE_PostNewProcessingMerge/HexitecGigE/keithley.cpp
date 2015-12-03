/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX

#include "keithley.h"
#include "windows.h"
#include "ni4882.h"
#include <limits>
#include <QStringList>
#include <QTime>
#include "objectreserver.h"

#define ARRAYSIZE          1024     // Size of read buffer
#define BDINDEX               0     // Board Index
//#define PRIMARY_ADDR_OF_PPS  24     // Primary address of device
#define NO_SECONDARY_ADDR     0     // Secondary address of device
#define TIMEOUT               T10s  // Timeout value = 10 seconds
#define EOTMODE               1     // Enable the END message
#define EOSMODE               0     // Disable the EOS mode

char buffer[ARRAYSIZE + 1];
char ErrorMnemonic[29][5] = { "EDVR", "ECIC", "ENOL", "EADR", "EARG",
                              "ESAC", "EABO", "ENEB", "EDMA", "",
                              "EOIP", "ECAP", "EFSO", "",     "EBUS",
                              "ESTB", "ESRQ", "",     "",      "",
                              "ETAB", "ELCK", "EARM", "EHDL",  "",
                              "",     "EWIP", "ERST", "EPWR" };


Keithley::Keithley(QObject *parent) :
   QThread(parent)
{
   biasRefreshEnabled = true;
   pollDisplayFlag = false;
   pollCurrentFlag = false;
   pollDisplayTime = 1;
   pollCurrentTime = 1;
   zeroSettings();

   // setup up the timers
   readTimer = new QTimer(this);
   connect(readTimer, SIGNAL(timeout()), this, SLOT(readData()));
   connect(this, SIGNAL(startReadTimerSignal()), this, SLOT(startReadTimer()));
   connect(this, SIGNAL(stopReadTimerSignal()), this, SLOT(stopReadTimer()));

   displayTimer = new QTimer(this);
   connect(displayTimer, SIGNAL(timeout()), this, SLOT(displayTimeout()));
   connect(this, SIGNAL(startDisplayTimerSignal()), this, SLOT(startDisplayTimer()));
   connect(this, SIGNAL(stopDisplayTimerSignal()), this, SLOT(stopDisplayTimer()));

   biasRefreshTimer = new QTimer(this);
   connect(biasRefreshTimer, SIGNAL(timeout()), this, SLOT(endOfRefresh()));
   connect(this, SIGNAL(startBiasRefreshTimerSignal()), this, SLOT(startBiasRefreshTimer()));
   connect(this, SIGNAL(stopBiasRefreshTimerSignal()), this, SLOT(stopBiasRefreshTimer()));

   connect(this, SIGNAL(startRefreshTimerSignal(double)), this, SLOT(startRefreshTimer(double)));
   connect(this, SIGNAL(stopRefreshTimerSignal()), this, SLOT(stopRefreshTimer()));

   biasSettleTimer = new QTimer(this);
   connect(biasSettleTimer, SIGNAL(timeout()), this, SLOT(endOfSettle()));
   connect(this, SIGNAL(startBiasSettleTimerSignal()), this, SLOT(startBiasSettleTimer()));
   connect(this, SIGNAL(stopBiasSettleTimerSignal()), this, SLOT(stopBiasSettleTimer()));

   executeBiasRefreshTimer = new QTimer(this);
   connect(executeBiasRefreshTimer, SIGNAL(timeout()), this, SLOT(executeBiasRefresh()));

   /*
    writeTimer = new QTimer(this);
    connect(writeTimer, SIGNAL(timeout()), this, SLOT(writeCommandFile()));
    connect(this, SIGNAL(startWriteTimerSignal()), this, SLOT(startWriteTimer()));
    connect(this, SIGNAL(stopWriteTimerSignal()), this, SLOT(stopWriteTimer()));
     */

   qRegisterMetaType<Keithley::VoltageSourceCommand>("Keithley::VoltageSourceCommand");

   rkl.append(this);

   return;
}

void Keithley::initialise(QString detectorFilename)
{
   detectorIniFile = new IniFile(detectorFilename);
   vb = detectorIniFile->getFloat("Bias Voltage/Bias Voltage");
   vr = detectorIniFile->getFloat("Bias Voltage/Refresh Voltage");
   vbrTime = detectorIniFile->getInt("Bias Voltage/Time Refresh Voltage Held");
   vbSettleTime = detectorIniFile->getInt("Bias Voltage/Bias Voltage Settle Time");
   currentLimit = detectorIniFile->getDouble("Bias Voltage/Current Limit");
   biasRefreshInterval = detectorIniFile->getInt("Bias Voltage/Bias Refresh Interval");
   totalBiasRefreshTime = vbrTime + vbSettleTime;
   //   biasRefreshInterval += totalBiasRefreshTime;
   voltage = vb;
   voltageAfterRefresh = voltage;
   GPIBwrite(":SOUR:VOLT:LEV " + QString::number(voltage));
   GPIBwrite(":SENS:FUNC \"CURR\""); // set the sense function to measure current
   // The current limit was originally hard coded to 1e-6. This was used to set the current
   // limit and the current measurement range. This value is now obtained from the ini file.
   GPIBwrite(":SENS:CURR:PROT " + QString::number(currentLimit)); // set the current limit
   GPIBwrite(":SENS:CURR:RANG " + QString::number(currentLimit)); // select the current measurement range
   _HVoff();
   if (vb < -500 || vb > 5)
   {
      emit vbOutOfRange();
   }
}

void Keithley::handleExecuteCommand(Keithley::VoltageSourceCommand command)
{
   Reservation reservation = ObjectReserver::instance()->reserveForGUI(rkl);

   if (reservation.getReserved().isEmpty())
   {
      emit writeError("Could not reserve all objects, message = " + reservation.getMessage());
   }
   else
   {
      if (command == HVON)
      {
          emit startRefreshTimerSignal(biasRefreshInterval);
         _HVset();
      }
      else if (command == HVOFF)
      {
         _HVoff();
         ObjectReserver::instance()->release(reservation.getReserved(), "GUIReserver");
      }
   }
}

int Keithley::getBiasRefreshTime()
{
   return totalBiasRefreshTime + 1000;
}

int Keithley::getBiasRefreshInterval()
{
   return biasRefreshInterval;
}

void Keithley::startRefreshTimer(double interval)
{
   enableBiasRefresh();
   executeBiasRefreshTimer->start(interval);
}

void Keithley::stopRefreshTimer()
{
   disableBiasRefresh();
   executeBiasRefreshTimer->stop();
}

double Keithley::getCurrent()
{
   bool validate;
   QString result;
   QStringList list;
   double current = std::numeric_limits<double>::quiet_NaN();
   int status;

   if (connectState)
   {
      status = getOutputStatus();
      if (status == 0)
      {
         return current = 0.0;
      }
      else if (status == 1)
      {
         ok = GPIBwrite(":READ?");
         ok = GPIBread();
         if (!ok)
         {
            readError(1);
            return current;
         }

         result = QString::fromLatin1(buffer);
         list = result.split(",");

         if (list.length() < 4)
         {
            readError(2);
            return current;
         }

         current = list[1].toDouble(&validate);
         if (!validate)
         {
            readError(2);
            current = std::numeric_limits<double>::quiet_NaN();
         }
      }
      else
      {
         readError(status);
         return current;
      }
   }

   return current;
}

bool Keithley::GPIBconnect(int PRIMARY_ADDR_OF_PPS)
{

   Dev = ibdev(BDINDEX, PRIMARY_ADDR_OF_PPS, NO_SECONDARY_ADDR,
               TIMEOUT, EOTMODE, EOSMODE);

   if (Ibsta() & ERR)
   {
      writeMessage("Unable to open device");
      writeMessage("nibsta = 0x" + QString::number(Ibsta()));
      writeMessage("iberr = " + QString::number(Iberr()));
      return false;
   }
   connectState = true;
   ok = GPIBwrite("*IDN?\n");
   if (!ok) return false;
   ok = GPIBread();
   if (!ok) return false;
   writeMessage("GPIB device found: "+ QString::fromLatin1(buffer));
   if (!QString::fromLatin1(buffer).contains("KEITHLEY"))
   {
      readError(99);
      return false;
   }
   flashMessage("HEXITEC operation",5000);
   defaultMeasureSettings();
   return true;
}

void Keithley::zeroSettings()
{
   voltage = 0;
   voltageAfterRefresh = voltage;
   biasRefreshState = false;
   currentLimit = 0;
   XData.resize(0);
   YData.resize(0);
   connectState = false;
}

void Keithley::defaultMeasureSettings()
{
   if (!connectState)
      return;
   biasRefreshState = false;
   //    voltage = -500.0;
   //    voltageAfterRefresh = voltage;
   //    currentLimit = 1*1E-6;
   GPIBwrite("*RST"); //re-set GPIB defaults
   GPIBwrite(":SOUR:FUNC VOLT"); //configure as voltage source
   GPIBwrite(":SOUR:VOLT:MODE FIX"); //fixed voltage source mode
   GPIBwrite(":SOUR:VOLT:RANG 100"); //set the displayed source range
   /*    GPIBwrite(":SOUR:VOLT:LEV " + QString::number(voltage));// %set the voltage to zero for now
    GPIBwrite(":SENS:FUNC \"CURR\""); //set the sense function to measure current
    GPIBwrite(":SENS:CURR:PROT " + QString::number(currentLimit));// %set the current limit
    GPIBwrite(":SENS:CURR:RANG " + QString::number(1E-6)); //select the 1 micro-Amp current measurement range
    */
}

void Keithley::GPIBdisconnect()
{
   // close the device
   ibonl(Dev, 0);
   connectState = false;
}

bool Keithley::GPIBwrite(QString cmd)
{
   if (!connectState)
      return false;
   ibwrt(Dev, cmd.toLocal8Bit(), (long)cmd.length());
   if (Ibsta() & ERR)
   {
      writeError("Unsuccessful GPIB write");
      return false;
   }
}

bool Keithley::GPIBread()
{
   if (!connectState)
      return false;
   ibrd(Dev,buffer,ARRAYSIZE);
   if (Ibsta() & ERR)
   {
      writeError("Unable to read Keithley");
      return false;
   }
   buffer[Ibcnt() - 1] = '\0';
}

bool Keithley::getDisplay()
{
   if (!connectState)
      return false;

   QString cmd;
   ok = GPIBwrite(":DISP:WIND1:DATA?");
   if (!ok) return false;
   ok = GPIBread();
   if (!ok) return false;
   return true;
}

bool Keithley::setDisplay(QString message)
{
   if (!connectState)
      return false;

   ok = GPIBwrite(":DISP:WIND1:TEXT:STAT 1");
   ok = GPIBwrite(":DISP:WIND1:TEXT:DATA \""+message+"\"");
   return ok;
}

bool Keithley::flashMessage(QString message, int messageDuration)
{
   ok = setDisplay(message);
   if (!ok)
   {
      return false;
   }
   displayDuration = messageDuration;
   emit startDisplayTimerSignal();
   return true;
}

void Keithley::pollCurrent(int pollTime)
{
   if (!connectState)
      return;

   // if already polling stop polling
   if (readTimer->isActive())
      emit stopReadTimerSignal();

   // if zero time do a single shot read
   if (pollTime == 0 & readTimer->isActive())
   {
      readData();
      return;
   }
   // start new poll
   pollCurrentTime = pollTime;
   pollCurrent();
}

void Keithley::pollCurrent()
{
   int status = getOutputStatus();
   if (status != 1)
   {
      readError(status);
      return;
   }
   emit startReadTimerSignal();
}

void Keithley::singleBiasRefresh()
{
   singleBiasRefresh(vbrTime);
}

void Keithley::singleBiasRefresh(int refreshTime)
{
   if (!connectState)
      return;

   if (biasOnState)
   {
      emit biasRefreshing();
      setDisplay("Bias refresh");
      voltageAfterRefresh = voltage;
      biasRefreshState = true;
      _HVset(vr);

      biasRefreshTime = refreshTime;
      emit startBiasRefreshTimerSignal();
   }
}

void Keithley::biasRefresh(int refreshTime)
{
   if (!connectState)
      return;

   if (biasOnState && biasRefreshEnabled)
   {
      emit biasRefreshing();
      setDisplay("Bias refresh");
      voltageAfterRefresh = voltage;
      biasRefreshState = true;
      _HVset(vr);
      biasRefreshTime = refreshTime;
      emit startBiasRefreshTimerSignal();
   }
}

void Keithley::biasRefresh()
{
   biasRefresh(vbrTime);
}

void Keithley::endOfRefresh()
{
   emit stopBiasRefreshTimerSignal();
   _HVset(voltageAfterRefresh);
   setDisplay("Bias Settle");
   emit startBiasSettleTimerSignal();
   //     biasRefreshState = false;
}

void Keithley::endOfSettle()
{
   emit stopBiasSettleTimerSignal();
   setDisplay();
   biasRefreshState = false;
   emit biasRefreshed(QTime::currentTime().toString());
}

void Keithley::HVset(double localVoltage)
{
   _HVset(localVoltage);
   emit biasVoltageChanged(true);
}

void Keithley::_HVset(double localVoltage)
{
   QMutexLocker locker(&mutex);

   if (localVoltage > 5 || localVoltage < -500)
   {
      writeError("Volatges hard coded to be in range -500 to +5 keV");
      writeWarning("Setting to 0 keV");
      localVoltage = 0;
   }
   voltage = localVoltage;
   _HVset();
}

void Keithley::_HVset()
{
   if (!connectState)
      return;

   // need to do some checks here to make sure its not at compliance
   ok = GPIBwrite(":OUTP ON");
   ok = GPIBwrite(":SOUR:VOLT:LEV " + QString::number(voltage));
   biasOnState = true;
   emit biasState(true);
   if (fabs(voltage - vr) < 0.0000001)
   {
      emit startRefreshTimerSignal(biasRefreshInterval + totalBiasRefreshTime);
   }
}

void Keithley::HVoff()
{
   _HVoff();
   emit biasVoltageChanged(false);
}

void Keithley::_HVoff()
{
   QMutexLocker locker(&mutex);

   if (!connectState)
      return;

   ok = GPIBwrite(":OUTP OFF");
   if (!ok)
   {
      readError(999);
   }
   else
   {
      biasOnState = false;
      emit biasState(false);
   }

   emit stopRefreshTimerSignal();
}

void Keithley::readData()
{
   if (!connectState)
      return;

   //   flashMessage("Reading...",(int) pollCurrentTime/2.0);
   bool validate;
   QString result;

   int status = getOutputStatus();
   if (status != 1)
   {
      readError(status);
      return;
   }
   //writeMessage("Output Status: "+QString::number(status));

   ok = GPIBwrite(":READ?");
   ok = GPIBread();
   if (!ok)
   {
      readError(1);
      return;
   }

   QStringList list;
   result = QString::fromLatin1(buffer);
   list = result.split(",");

   if (list.length() < 4)
   {
      readError(2);
      return;
   }

   double convertedX = list[3].toDouble(&validate);
   double convertedY = list[1].toDouble(&validate);
   if (validate)
   {
      XData.push_back(convertedX);
      YData.push_back(convertedY/1e6);
   }
   else
   {
      readError(2);
      return;
   }
   emit plotValues(XData,YData,0);
   //    for (int i = 0; i < XData.size(); ++i)
   //        writeMessage("XY: "+ QString::number(XData[i])+ "," +QString::number(YData[i]));

}

int Keithley::getOutputStatus()
{
   if (!connectState)
      return(-1);

   bool validate;
   QString result;
   ok = GPIBwrite(":OUTP:STAT?");
   ok = GPIBread();
   result = QString::fromLatin1(buffer);
   int status = result.toInt(&validate);
   if (validate)
      return(status);
   return(-1);
}

void Keithley::readError(int error)
{
   if (error == -1)
      writeError("Output status cannot be read");
   if (error == 0)
      writeError("Output is off");
   if (error == 1)
      writeError("General read error");
   if (error == 2)
      writeError("Data read error");
   if (error == 99)
      writeError("Keithley not found");
   if (error == 999)
   {
      voltage = 0;
      writeError("Cannot switch off output attemping to set voltage to zero...");
      ok = GPIBwrite(":SOUR:VOLT:LEV " + QString::number(voltage));
   }
   // kill the polling
   if (readTimer->isActive())
   {
      writeMessage("Stopping current polling");
      emit stopReadTimerSignal();
   }
   // emit

}

void Keithley::startBiasRefreshTimer()
{
   biasRefreshTimer->start(biasRefreshTime);
}

void Keithley::stopBiasRefreshTimer()
{
   biasRefreshTimer->stop();
}

void Keithley::startBiasSettleTimer()
{
   biasSettleTimer->start(vbSettleTime);
}

void Keithley::stopBiasSettleTimer()
{
   biasSettleTimer->stop();
}

void Keithley::startReadTimer()
{
   readTimer->start(pollCurrentTime);
}

void Keithley::stopReadTimer()
{
   readTimer->stop();
}

void Keithley::startDisplayTimer()
{
   displayTimer->start(displayDuration);
}

void Keithley::stopDisplayTimer()
{
   displayTimer->stop();
}

void Keithley::displayTimeout()
{
   emit stopDisplayTimerSignal();
   if (!connectState)
      return;
   setDisplay();
}

void Keithley::setDisplay()
{
   ok = GPIBwrite(":DISP:WIND1:TEXT:STAT 0");
   ok = GPIBwrite(":DISP:CNDisplay");
}

void Keithley::handleTemperatureBelowDP()
{
   _HVoff();
   disableBiasRefresh();
}

void Keithley::biasOn()
{
   _biasOn();
   emit biasVoltageChanged(true);
}

void Keithley::_biasOn()
{
   //   QMutexLocker locker(&mutex);

   _HVset(vb);
}

void Keithley::enableBiasRefresh()
{
   biasRefreshEnabled = true;
}

void Keithley::disableBiasRefresh()
{
   biasRefreshEnabled = false;
}

bool Keithley::refreshIsEnabled()
{
   return biasRefreshEnabled;
}

void Keithley::executeBiasRefresh()
{
   biasRefresh();
}

void Keithley::executeSingleBiasRefresh()
{
   singleBiasRefresh();
}

void Keithley::storeBiasSettings()
{
   storedBiasOn = biasOnState;
   storedBiasRefreshing = biasRefreshEnabled;
}

void Keithley::restoreBiasSettings()
{
   biasOnState = storedBiasOn;
   biasRefreshEnabled = storedBiasRefreshing;
   if (biasRefreshEnabled)
   {
      enableBiasRefresh();
   }
}

bool Keithley::getBiasOnState()
{
   return biasOnState;
}

void Keithley::handleDisableBiasRefresh()
{
   disableBiasRefresh();
}

/*
void Keithley::startWriteTimer()
{
   writeTimer->start(1000);
}

void Keithley::stopWriteTimer()
{
   writeTimer->stop();
}

*/

//#include <C:\Program Files (x86)\National Instruments\NI-DAQ\DAQmx ANSI C Dev\include\NIDAQmx.h>
//#include "NIDAQmx.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include "C:/Program Files (x86)/National Instruments/Shared/ExternalCompilerSupport/C/include/ni488.h"
//#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
/*
  int         error=0;
  TaskHandle	taskHandle=0;
  uInt32      data;
  char        errBuff[2048]={'\0'};
  int32		written;

  // DAQmx Configure Code
  DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
  DAQmxErrChk (DAQmxCreateDOChan(taskHandle,"GPIB0/port7","",DAQmx_Val_ChanForAllLines));

  // DAQmx Start Code
  DAQmxErrChk (DAQmxStartTask(taskHandle));

  // DAQmx Write Code
  DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));

  writeMessage("Keithley might be doing something");

Error:
  if( DAQmxFailed(error) )
      DAQmxGetExtendedErrorInfo(errBuff,2048);
  if( taskHandle!=0 ) {
      // DAQmx Stop Code
      DAQmxStopTask(taskHandle);
      DAQmxClearTask(taskHandle);
  }
  if( DAQmxFailed(error) )
      writeError("DAQmx Error: "+QString::fromAscii(errBuff));
*/
