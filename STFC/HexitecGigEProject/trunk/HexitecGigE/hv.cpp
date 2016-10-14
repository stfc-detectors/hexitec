#include <locale>
#include "hv.h"
#include "objectreserver.h"

HV::HV(QObject *parent) :
    QObject(parent)
{
    biasRefreshEnabled = true;

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
   emit stopRefreshTimerSignal();
}

void HV::on(double voltage)
{
    this->voltage = voltage;
    emit setHV(voltage);
    biasOnState = true;
    emit biasState(true);
    if (fabs(voltage - vr) < 0.0000001)
    {
       emit startRefreshTimerSignal(biasRefreshInterval + totalBiasRefreshTime);
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
    qDebug() << "hv doing singleBiasRefresh!!!";
    if (biasOnState)
    {
       emit biasRefreshing();
       qDebug() << "hv doing the refresh!!!";
       voltageAfterRefresh = voltage;
       biasRefreshState = true;
       on(vr);

       biasRefreshTime = refreshTime;
       emit startBiasRefreshTimerSignal();
    }

}

void HV::singleBiasRefresh()
{
    singleBiasRefresh(vbrTime);
}

void HV::biasRefresh(int refreshTime)
{
    if (biasOnState && biasRefreshEnabled)
    {
       emit biasRefreshing();
       voltageAfterRefresh = voltage;
       biasRefreshState = true;
       on(vr);
       biasRefreshTime = refreshTime;
       emit startBiasRefreshTimerSignal();
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
          emit startRefreshTimerSignal(biasRefreshInterval);
          on();
       }
       else if (command == HVOFF)
       {
          emit stopRefreshTimerSignal();
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
    emit stopBiasRefreshTimerSignal();
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

void HV::startRefreshTimer(double interval)
{
   qDebug() <<"HV::startRefreshTimer, interval: " << interval;
    enableBiasRefresh();
    executeBiasRefreshTimer->start(interval);
}

void HV::stopRefreshTimer()
{
   qDebug() <<"HV::stopRefreshTimer";
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

void HV::startBiasRefreshTimer()
{
   biasRefreshTimer->start(biasRefreshTime);
}

void HV::stopBiasRefreshTimer()
{
   biasRefreshTimer->stop();
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
