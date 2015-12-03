#ifndef KEITHLEY_H
#define KEITHLEY_H

#include <QObject>
#include <QTimer>
#include <QVariant>
#include <QThread>
#include <QDebug>
#include <QString>
#include "reservable.h"
#include "inifile.h"

class Keithley: public QThread, public Reservable
{
   Q_OBJECT

public:
   enum VoltageSourceCommand {HVON, HVOFF};

   explicit Keithley(QObject *parent = 0);
   void test() {writeMessage("This is a test message from Keithley class");}
   bool GPIBconnect(int);
   void GPIBdisconnect();
   bool GPIBwrite(QString);
   bool GPIBread();
   bool flashMessage(QString, int);
   bool setDisplay(QString);
   bool getDisplay();
   void defaultMeasureSettings();
   void zeroSettings();
   void pollCurrent(int);
   Q_INVOKABLE void HVset(double);
   void _HVset(double);
   Q_INVOKABLE void HVoff();
   void _HVoff();
   int getOutputStatus();
   void biasRefresh(int);
   void singleBiasRefresh(int);
   double getSetVoltage() {return(voltage);}
   bool biasRefreshStatus() {return biasRefreshState;}
   void initialise(QString detectorFilename);
   void biasRefresh();
   void singleBiasRefresh();
   Q_INVOKABLE void biasOn();
   void _biasOn();
   void enableBiasRefresh();
   void disableBiasRefresh();
   bool refreshIsEnabled();
   int getBiasRefreshTime();
   int getBiasRefreshInterval();
   double getCurrent();
   bool getBiasOnState();

signals:
   void writeMessage(QString);
   void writeWarning(QString);
   void writeError(QString);
   void startReadTimerSignal();
   void stopReadTimerSignal();
   void startDisplayTimerSignal();
   void stopDisplayTimerSignal();
   void startRefreshTimerSignal(double interval);
   void stopRefreshTimerSignal();
   void startBiasRefreshTimerSignal();
   void stopBiasRefreshTimerSignal();
   void startBiasSettleTimerSignal();
   void stopBiasSettleTimerSignal();
   void plotValues(QVector <double> &, QVector <double> &, bool);
   void biasRefreshing();
   void biasRefreshed(QString);
   void vbOutOfRange();
   void biasState(bool);
   void biasVoltageChanged(bool);
   /*
    void startWriteTimerSignal();
    void stopWriteTimerSignal();
    */

private slots:
   void handleExecuteCommand(Keithley::VoltageSourceCommand command);
   void readData();
   void displayTimeout();
   void endOfRefresh();
   void endOfSettle();
   void startReadTimer();
   void stopReadTimer();
   void startDisplayTimer();
   void stopDisplayTimer();
   void stopBiasRefreshTimer();
   void startBiasSettleTimer();
   void stopBiasSettleTimer();
   void executeBiasRefresh();
   void executeSingleBiasRefresh();
   void storeBiasSettings();
   void restoreBiasSettings();
   void handleDisableBiasRefresh();
   void handleTemperatureBelowDP();
   void startRefreshTimer(double interval);
   void stopRefreshTimer();

public slots:
   void startBiasRefreshTimer();

private:
   void setDisplay();
   int Dev;
   bool connectState;
   bool pollDisplayFlag;
   bool pollCurrentFlag;
   bool ok;
   int pollDisplayTime;
   int displayDuration;
   int pollCurrentTime;
   int biasRefreshTime;
   int totalBiasRefreshTime;
   double voltage;
   double voltageAfterRefresh;
   double currentLimit;
   QTimer *readTimer;
   QTimer *displayTimer;
   QTimer *biasRefreshTimer;
   QTimer *biasSettleTimer;
   QTimer *executeBiasRefreshTimer;
   bool biasRefreshState;
   double hV;
   double vb;
   double vr;
   int vbrTime;
   int vbSettleTime;
   int biasRefreshInterval;
   IniFile *detectorIniFile;
   void pollCurrent();
   void _HVset();
   void readError(int);
   QVector <double> XData;
   QVector <double> YData;
   bool biasRefreshEnabled;
   bool biasOnState;
   bool storedBiasOn;
   bool storedBiasRefreshing;
   QMutex mutex;
   QList <QObject *> rkl;
};

#endif // KEITHLEY_H
