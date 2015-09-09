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
/*    GPIBwrite(":SOUR:VOLT:LEV " + QString::number(voltage));
    GPIBwrite(":SENS:FUNC \"CURR\""); // set the sense function to measure current
    // The current limit was originally hard coded to 1e-6. This was used to set the current
    // limit and the current measurement range. This value is now obtained from the ini file.
    GPIBwrite(":SENS:CURR:PROT " + QString::number(currentLimit)); // set the current limit
    GPIBwrite(":SENS:CURR:RANG " + QString::number(currentLimit)); // select the current measurement range
*/
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

void HV::off()
{
   emit setHV(0.0);
   biasOnState = false;
   emit biasState(false);
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
    if (biasOnState)
    {
       emit biasRefreshing();
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
          off();
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
   biasRefreshEnabled = false;
}

void HV::handleTemperatureBelowDP()
{
    off();
    disableBiasRefresh();

}

void HV::executeBiasRefresh()
{
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

void HV::handleDisableBiasRefresh()
{
    disableBiasRefresh();

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
    enableBiasRefresh();
    executeBiasRefreshTimer->start(interval);
}

void HV::stopRefreshTimer()
{
    disableBiasRefresh();
    executeBiasRefreshTimer->stop();
}

bool HV::getBiasOnState()
{
    return biasOnState;
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
