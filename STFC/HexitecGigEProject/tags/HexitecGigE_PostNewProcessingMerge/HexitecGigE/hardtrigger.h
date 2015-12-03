#ifndef HARDTRIGGER_H
#define HARDTRIGGER_H

#include <QObject>
#include <QTimer>
#include <QVariant>
#include <QThread>
#include <QDebug>
#include "reservable.h"


class HardTrigger : public QThread, public Reservable
{
    Q_OBJECT
public:
    explicit HardTrigger(QObject *parent = 0);
    Q_INVOKABLE void sendPulse();
    Q_INVOKABLE void sendPulse(int);
    Q_INVOKABLE void setPort(QString value);
    Q_INVOKABLE bool getState() {return(triggerState);}
   // Q_INVOKABLE int getState() { if (triggerState) return(1); else return(0); }

signals:
    void writeMessage(QString);
    void writeWarning(QString);
    void writeError(QString);
    void startSleepTimerSignal();
    void stopSleepTimerSignal();

private slots:
    void triggerTrue() {triggerState = true; trigger();}
    void triggerFalse() {triggerState = false; trigger();}
    void startSleepTimer();
    void stopSleepTimer();
    void sleepEnd();

public slots:

private:
    void trigger();
    const char* taskAddress;
    bool triggerState;
    int defaultPulseTime;
    QString portAddress;
    QTimer *sleepTimer;
    int sleepTime;

protected:
    QMutex mutex;

};

#endif // HARDTRIGGER_H
