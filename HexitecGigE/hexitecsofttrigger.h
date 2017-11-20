#ifndef HEXITECSOFTTRIGGER_H
#define HEXITECSOFTTRIGGER_H

#include <QObject>
#include <QTimer>
#include <QFile>
#include <QVariant>
#include <QThread>
#include "reservable.h"

class HexitecSoftTrigger : public QThread, public Reservable
{
   Q_OBJECT

public:
    explicit HexitecSoftTrigger(QObject *parent = 0);
    Q_INVOKABLE void setCommandFileName(QString value) {commandFileName = value; commandFile->setFileName(commandFileName); }
    Q_INVOKABLE void setStatusFileName(QString value) {statusFileName = value; statusFile->setFileName(statusFileName); }
    Q_INVOKABLE void setDataFileLocations(QString value) { dataFileLocations = value; }
    Q_INVOKABLE void setDataFilePrefix(QString value) { dataFilePrefix = value; setNextFileName();}
    Q_INVOKABLE void setCountTimeMS(unsigned int value) { countTimeMS = value; }
    Q_INVOKABLE void setCountTimeSEC(unsigned int value) { countTimeMS = value * 1000; }
    Q_INVOKABLE void setFileNumber(unsigned int value) { fileNumber = value; setNextFileName();}
    Q_INVOKABLE void liveReadOn() { liveRead = true; }
    Q_INVOKABLE void liveReadOff() { liveRead = false; }
    Q_INVOKABLE void acquire(unsigned int);
    Q_INVOKABLE void currentSettings();
    Q_INVOKABLE void kill();
    Q_INVOKABLE void test() {emit writeMessage("This is a test message from HexitecSoftTrigger instance");}
    Q_INVOKABLE void whatsGoingOn() {verbose = true; emit writeMessage("Verbose Mode");}
    Q_INVOKABLE void whatsGoingOff(){verbose = false; emit writeMessage("Verbose Mode Off");}
    Q_INVOKABLE bool getStatus() {return(state);}
    Q_INVOKABLE void sleep(int);
    Q_INVOKABLE void sleepEnd();
    Q_INVOKABLE bool getSleepStatus() {return(sleepState);}

signals:
    void writeMessage(QString);
    void writeWarning(QString);
    void writeError(QString);
    void readLastAcquiredFile(QString);
    void startWriteTimerSignal();
    void stopWriteTimerSignal();
    void startReadTimerSignal();
    void stopReadTimerSignal();
    // temporary sleep timer stuff
    void startSleepTimerSignal();
    void stopSleepTimerSignal();

public slots:
    Q_INVOKABLE void acquire();

private slots:
    void writeCommandFile();
    void readStatusFile();
    void startWriteTimer();
    void stopWriteTimer();
    void startReadTimer();
    void stopReadTimer();
    // temporary sleep timer stuff
    void startSleepTimer();
    void stopSleepTimer();

private:
    QString triggerFileLocations;
    QString dataFileLocations;
    QString EOLcode;
    QString dataFilePrefix;
    unsigned int countTimeMS;
    unsigned int fileNumber;
    QString nextFileToWrite;
    QString lastFileWritten;
    void setNextFileName();
    bool liveRead;
    bool state;
    int pollTime;
    QTimer *writeTimer;
    QTimer *readTimer;
    // temporary sleep timer stuff
    QTimer *sleepTimer;
    bool sleepState;
    int sleepTime;
    QFile *commandFile;
    QFile *statusFile;
    QString commandFileName;
    QString statusFileName;
    bool verbose;

protected:
    QMutex mutex;

};

#endif // HEXITECSOFTTRIGGER_H
