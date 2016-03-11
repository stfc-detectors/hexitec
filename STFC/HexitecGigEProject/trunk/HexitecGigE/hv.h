#ifndef HV_H
#define HV_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include "inifile.h"
#include "reservable.h"

class HV : public QObject, public Reservable
{
    Q_OBJECT
public:
    enum VoltageSourceCommand {HVON, HVOFF};

    explicit HV(QObject *parent = 0);
    void initialise(QString detectorFilename);
    int getBiasRefreshTime();
    int getBiasRefreshInterval();
    bool getBiasOnState();
    Q_INVOKABLE void off();
    Q_INVOKABLE void on(double voltage);
    Q_INVOKABLE void on();
    void biasRefresh(int);
    void biasRefresh();
    void singleBiasRefresh(int refreshTime);
    void singleBiasRefresh();
    void enableBiasRefresh();
    void disableBiasRefresh();

private:
    double voltage;
    double voltageAfterRefresh;
    double currentLimit;
    bool biasRefreshState;
    double hV;
    double vb;
    double vr;
    int vbrTime;
    int vbSettleTime;
    int biasRefreshInterval;
    int totalBiasRefreshTime;
    int biasRefreshTime;
    QTimer *biasRefreshTimer;
    QTimer *biasSettleTimer;
    QTimer *executeBiasRefreshTimer;
    bool biasRefreshEnabled;
    bool biasOnState;
    bool storedBiasOn;
    bool storedBiasRefreshing;
    IniFile *detectorIniFile;
    QList <QObject *> rkl;

signals:
    void writeMessage(QString);
    void writeWarning(QString);
    void writeError(QString);
    void setHV(double voltage);
    void startBiasRefreshTimerSignal();
    void stopBiasRefreshTimerSignal();
    void startRefreshTimerSignal(double interval);
    void stopRefreshTimerSignal();
    void startBiasSettleTimerSignal();
    void stopBiasSettleTimerSignal();
    void biasRefreshing();
    void biasRefreshed(QString);
    void biasState(bool);
    void biasVoltageChanged(bool);
    void vbOutOfRange();

private slots:
    void handleExecuteCommand(HV::VoltageSourceCommand command);
    void handleTemperatureBelowDP();
    void executeBiasRefresh();
    void executeSingleBiasRefresh();
    void storeBiasSettings();
    void restoreBiasSettings();
    void handleDisableBiasRefresh();
    void handleEnableBiasRefresh();
    void endOfRefresh();
    void endOfSettle();
    void startRefreshTimer(double interval);
    void stopRefreshTimer();
    void stopBiasRefreshTimer();
    void startBiasSettleTimer();
    void stopBiasSettleTimer();

public slots:
   void startBiasRefreshTimer();

};

#endif // HV_H
