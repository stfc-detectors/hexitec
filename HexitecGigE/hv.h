#ifndef HV_H
#define HV_H

#include <QObject>
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
    int getTotalBiasRefreshInterval();
    bool getBiasOnState();
    bool getBiasPriority();
    bool getStoredBiasOn();
    Q_INVOKABLE void off();
    Q_INVOKABLE void on(double voltage);
    Q_INVOKABLE void on();
    void biasRefresh(int);
    void biasRefresh();
    void singleBiasRefresh(int refreshTime);
    void singleBiasRefresh();
    void enableBiasRefresh();
    void disableBiasRefresh();
    void setReadyForRefresh(bool readyForRefresh);

private:
    double voltage;
    double voltageAfterRefresh;
    double currentLimit;
    bool readyForRefresh;
    bool biasRefreshState;
    bool biasPriority; //(LOW = false, HIGH = true)
    bool restartMonitoring;
    double hV;
    double vb;
    double vr;
    int vbrTime;
    int vbSettleTime;
    int biasRefreshInterval;
    int totalBiasRefreshTime;
    int biasRefreshTime;
    QTimer *biasRefreshHeldTimer;
    QTimer *biasSettleTimer;
    QTimer *biasRefreshIntervalTimer;
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
    void startBiasRefreshHeldSignal();
    void stopBiasRefreshHeldSignal();
    void startExecuteBiasRefreshTimerSignal(double interval);
    void stopExecuteBiasRefreshTimerSignal();
    void startBiasSettleTimerSignal();
    void stopBiasSettleTimerSignal();
    void biasRefreshing();
    void biasRefreshed(QString, bool);
    void biasState(bool);
    void biasVoltageChanged(bool);
    void vbOutOfRange();
    void prepareForBiasRefresh();
    void suspendMonitoring();

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
    void startExecuteBiasRefreshTimer(double interval);
    void stopExecuteBiasRefreshTimer();
    void stopBiasRefreshHeldTimer();
    void startBiasSettleTimer();
    void stopBiasSettleTimer();
    void waitForReady();

public slots:
   void startBiasRefreshHeldTimer();

};

#endif // HV_H
