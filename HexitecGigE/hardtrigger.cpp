#include "hardtrigger.h"
#include "NIDAQmx.h"

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

HardTrigger::HardTrigger(QObject *parent) :
   QThread(parent)
{
   defaultPulseTime = 500;
   triggerState = false;
   sleepTimer = new QTimer(this);
   connect(sleepTimer, SIGNAL(timeout()), this, SLOT(sleepEnd()));
   connect(this, SIGNAL(startSleepTimerSignal()), this, SLOT(startSleepTimer()));
   connect(this, SIGNAL(stopSleepTimerSignal()), this, SLOT(stopSleepTimer()));

}

void HardTrigger::startSleepTimer()
{
   sleepTimer->start(sleepTime);
}

void HardTrigger::stopSleepTimer()
{
   sleepTimer->stop();
}

void HardTrigger::sleepEnd()
{
   emit stopSleepTimerSignal();
   trigger();
}

void HardTrigger::sendPulse()
{
   sendPulse(defaultPulseTime);
}

void HardTrigger::sendPulse(int pulseTime)
{
   sleepTime = pulseTime;
   trigger();
   emit startSleepTimerSignal();
   return;
}

void HardTrigger::setPort(QString value)
{
}

void HardTrigger::trigger()
{
   int         error=0;
   TaskHandle	taskHandle=0;
   uInt32      data;
   char        errBuff[2048]={'\0'};
   int32		written;

   if (triggerState)
      data = 0xffffffff;
   else
      data = 0;

   // DAQmx Configure Code
   DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
   DAQmxErrChk (DAQmxCreateDOChan(taskHandle,"Dev3/port0","",DAQmx_Val_ChanForAllLines));

   // DAQmx Start Code
   DAQmxErrChk (DAQmxStartTask(taskHandle));

   // DAQmx Write Code
   DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
   triggerState = !triggerState;

Error:
   if( DAQmxFailed(error) )
      DAQmxGetExtendedErrorInfo(errBuff,2048);
   if( taskHandle!=0 ) {
      // DAQmx Stop Code
      DAQmxStopTask(taskHandle);
      DAQmxClearTask(taskHandle);
   }
   if( DAQmxFailed(error) )
      writeError("DAQmx Error: " + QString::fromLocal8Bit(errBuff));

   if (triggerState)
      writeMessage("Trigger ON\n");
   else
      writeMessage("Trigger OFF\n");

   return;
}

/*
  Galil October 2011 Code
  see C:\Users\Simon\Desktop\toTidy\to_tidy\GALIL_OCT11 on Simon's work PC
#include <C:\Program Files (x86)\National Instruments\NI-DAQ\DAQmx ANSI C Dev\include\NIDAQmx.h>
void trigger(bool);
#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
*/
/*
  Galil October 2011 Code

    int         error=0;
    TaskHandle	taskHandle=0;
    uInt32      data;
    char        errBuff[2048]={'\0'};
    int32		written;

    if (triggerState)
        data = 0xffffffff;
    else
        data = 0;


    // DAQmx Configure Code
    DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
    DAQmxErrChk (DAQmxCreateDOChan(taskHandle,"Dev1/port0","",DAQmx_Val_ChanForAllLines));

    // DAQmx Start Code
    DAQmxErrChk (DAQmxStartTask(taskHandle));

    // DAQmx Write Code
    DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));

Error:
    if( DAQmxFailed(error) )
        DAQmxGetExtendedErrorInfo(errBuff,2048);
    if( taskHandle!=0 ) {
        // DAQmx Stop Code
        DAQmxStopTask(taskHandle);
        DAQmxClearTask(taskHandle);
    }
    if( DAQmxFailed(error) )
        printf("DAQmx Error: %s\n",errBuff);

    if (triggerState)
        printf("Trigger ON\n");
    else
        printf("Trigger OFF\n");

    return;

 */
