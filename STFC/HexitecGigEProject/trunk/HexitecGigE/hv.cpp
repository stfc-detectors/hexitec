#include <locale>
#include "hv.h"
#include "objectreserver.h"

HV::HV(QObject *parent) :
    QObject(parent)
{
    biasRefreshEnabled = true;

    biasRefreshHeldTimer = new QTimer(this);
    biasRefreshHeldTimer->setSingleShot(true);
    connect(biasRefreshHeldTimer, SIGNAL(timeout()), this, SLOT(endOfRefresh()));
    connect(this, SIGNAL(startBiasRefreshHeldSignal()), this, SLOT(startBiasRefreshHeldTimer()));
    connect(this, SIGNAL(stopBiasRefreshHeldSignal()), this, SLOT(stopBiasRefreshHeldTimer()));

    connect(this, SIGNAL(startExecuteBiasRefreshTimerSignal(double)), this, SLOT(startExecuteBiasRefreshTimer(double)));
    connect(this, SIGNAL(stopExecuteBiasRefreshTimerSignal()), this, SLOT(stopExecuteBiasRefreshTimer()));

    biasSettleTimer = new QTimer(this);
    biasSettleTimer->setSingleShot(true);
    connect(biasSettleTimer, SIGNAL(timeout()), this, SLOT(endOfSettle()));
    connect(this, SIGNAL(startBiasSettleTimerSignal()), this, SLOT(startBiasSettleTimer()));
    connect(this, SIGNAL(stopBiasSettleTimerSignal()), this, SLOT(stopBiasSettleTimer()));

    executeBiasRefreshTimer = new QTimer(this);
    executeBiasRefreshTimer->setSingleShot(true);
    connect(executeBiasRefreshTimer, SIGNAL(timeout()), this, SLOT(executeBiasRefresh()));

    qRegisterMetaType<HV::VoltageSourceCommand>("HV::VoltageSourceCommand");

    rkl.append(this);
}

void HV::initialise(QString detectorFilename)
{
   QString vbPriority;
    detectorIniFile = new IniFile(detectorFilename);

    vbPriority = detectorIniFile->getString("Bias_Voltage/Bias_Voltage_Priority").toUpper();
    if (vbPriority == "HIGH")
    {
       qDebug() << "Setting bias priority TRUE";
       biasPriority = true;
    }
    else if (vbPriority == "LOW")
    {
       qDebug() << "Setting bias priority FALSE";
       biasPriority = false;
    }
    vb = detectorIniFile->getFloat("Bias_Voltage/Bias_Voltage");
    vr = detectorIniFile->getFloat("Bias_Voltage/Refresh_Voltage");
    vbrTime = detectorIniFile->getInt("Bias_Voltage/Time_Refresh_Voltage_Held");
    vbSettleTime = detectorIniFile->getInt("Bias_Voltage/Bias_Voltage_Settle_Time");
    currentLimit = detectorIniFile->getDouble("Bias_Voltage/Current_Limit");
    biasRefreshInterval = detectorIniFile->getInt("Bias_Voltage/Bias_Refresh_Interval");
    totalBiasRefreshTime = vbrTime + vbSettleTime;
    voltage = vb;
    voltageAfterRefresh = voltage;
    storedBiasOn = false;
    readyForRefresh = true;

    off();
    if (vb < -500 || vb > 5)
    {
       emit vbOutOfRange();
    }

}

int HV::getBiasRefreshTime()
{
    return totalBiasRefreshTime + 1000;
}

int HV::getBiasRefreshInterval()
{
   return biasRefreshInterval;
}

int HV::getTotalBiasRefreshInterval()
{
   return totalBiasRefreshTime + biasRefreshInterval;
}

void HV::off()
{
   emit setHV(0.0);
   biasOnState = false;
   emit biasState(false);
   emit stopExecuteBiasRefreshTimerSignal();
}

void HV::on(double voltage)
{
    this->voltage = voltage;
    emit setHV(voltage);
    biasOnState = true;
    emit biasState(true);
    if ((fabs(voltage - vr) < 0.0000001) && (!executeBiasRefreshTimer->isActive()))
    {
//       qDebug() << QTime::currentTime().toString() << "emit startRefreshTimerSignal from HV::on(), biasRefreshTimer->isActive()"
//                << executeBiasRefreshTimer->isActive();
       emit startExecuteBiasRefreshTimerSignal(biasRefreshInterval + totalBiasRefreshTime);
    }
}

void HV::on()
{
    on(voltage);
}

void HV::biasRefresh()
{
    biasRefresh(vbrTime);
}

void HV::singleBiasRefresh(int refreshTime)
{
//    qDebug() << QTime::currentTime().toString() << "hv doing singleBiasRefresh!!!";
    if (biasOnState)
    {
       emit biasRefreshing();
//       qDebug() << QTime::currentTime().toString() << "hv doing the refresh!!!";
       voltageAfterRefresh = voltage;
       biasRefreshState = true;
       on(vr);

       biasRefreshTime = refreshTime;
       emit startBiasRefreshHeldSignal();
    }

}

void HV::singleBiasRefresh()
{
    singleBiasRefresh(vbrTime);
}

void HV::biasRefresh(int refreshTime)
{
//   qDebug() << QTime::currentTime().toString() << "HV::biasRefresh() called";
    if (biasOnState && biasRefreshEnabled)
    {
       emit biasRefreshing();
       voltageAfterRefresh = voltage;
       biasRefreshState = true;
//       qDebug() << QTime::currentTime().toString() << "calling HV::on() with " << vr;
       on(vr);
       biasRefreshTime = refreshTime;
       emit startBiasRefreshHeldSignal();
    }

}

void HV::handleExecuteCommand(HV::VoltageSourceCommand command)
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
          qDebug() << QTime::currentTime().toString() << "emit startRefreshTimerSignal from HV::handleExecuteCommand()";
          emit startExecuteBiasRefreshTimerSignal(biasRefreshInterval);
          on();
       }
       else if (command == HVOFF)
       {
          emit stopExecuteBiasRefreshTimerSignal();
          off();
          //qDebug() << "Turning HV off and releasing!";
          ObjectReserver::instance()->release(reservation.getReserved(), "GUIReserver");
       }
    }

}

void HV::enableBiasRefresh()
{
   biasRefreshEnabled = true;
}

void HV::disableBiasRefresh()
{
   if (biasPriority)
   {
      biasRefreshEnabled = false;
   }
   else
   {
      readyForRefresh = false;
   }
}

void HV::setReadyForRefresh(bool readyForRefresh)
{
   this->readyForRefresh = readyForRefresh;
}

void HV::handleTemperatureBelowDP()
{
    off();
    disableBiasRefresh();

}

void HV::executeBiasRefresh()
{
   if (!biasPriority)
   {
      qDebug() << "prepare for a refresh, readyForRefresh = " << readyForRefresh;
      emit prepareForBiasRefresh();
      waitForReady();
      qDebug() << "readyForRefresh = " << readyForRefresh;
   }
   biasRefresh();
}

void HV::executeSingleBiasRefresh()
{
    singleBiasRefresh();
}

void HV::storeBiasSettings()
{
    storedBiasOn = biasOnState;
    storedBiasRefreshing = biasRefreshEnabled;
    qDebug() << "storeBiasSettings(), biasOnState: " << biasOnState << " biasRefreshEnabled: " << biasRefreshEnabled;
}

void HV::restoreBiasSettings()
{
    biasOnState = storedBiasOn;
    biasRefreshEnabled = storedBiasRefreshing;
    if (biasRefreshEnabled)
    {
       enableBiasRefresh();
    }

}

bool HV::getStoredBiasOn()
{
   return storedBiasOn;
}
void HV::handleDisableBiasRefresh()
{
    disableBiasRefresh();
}

void HV::handleEnableBiasRefresh()
{
    enableBiasRefresh();
}

void HV::endOfRefresh()
{
   qDebug() << QTime::currentTime().toString() << " endOfRefresh(), calling HV::on() with " << voltageAfterRefresh;
   emit stopBiasRefreshHeldSignal();
    on(voltageAfterRefresh);
    emit startBiasSettleTimerSignal();
    //     biasRefreshState = false;

}

void HV::endOfSettle()
{
    emit stopBiasSettleTimerSignal();
    biasRefreshState = false;
    emit biasRefreshed(QTime::currentTime().toString());
}

void HV::startExecuteBiasRefreshTimer(double interval)
{
//    qDebug() << QTime::currentTime().toString() <<"HV::startRefreshTimer, interval: " << interval;
    enableBiasRefresh();
    executeBiasRefreshTimer->start(interval);
}

void HV::stopExecuteBiasRefreshTimer()
{
//   qDebug() <<"HV::stopRefreshTimer";
    disableBiasRefresh();
    executeBiasRefreshTimer->stop();
}

bool HV::getBiasOnState()
{
   return biasOnState;
}

bool HV::getBiasPriority()
{
   return biasPriority;
}

void HV::startBiasRefreshHeldTimer()
{
//   qDebug() << QTime::currentTime().toString() << "startBiasRefreshHeldTimer() with " << biasRefreshTime;
   biasRefreshHeldTimer->start(biasRefreshTime);
}

void HV::stopBiasRefreshHeldTimer()
{
   biasRefreshHeldTimer->stop();
}

void HV::startBiasSettleTimer()
{
   biasSettleTimer->start(vbSettleTime);
}

void HV::stopBiasSettleTimer()
{
   biasSettleTimer->stop();
}

void HV::waitForReady()
{
   while (!readyForRefresh)
   {
      QThread::sleep(0.1);
   }
}
