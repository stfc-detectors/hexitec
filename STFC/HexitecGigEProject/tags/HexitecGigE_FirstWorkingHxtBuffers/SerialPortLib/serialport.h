#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QHash>
#include "qextserialport.h"

class SerialPort : public QObject
{
    Q_OBJECT

public:
//    SerialPort(QObject *parent);
    explicit SerialPort(QString port, QObject *parent = 0);
    Q_INVOKABLE QString writeTermination();
    Q_INVOKABLE QString readTermination();
    Q_INVOKABLE void setReadTermination(QString readTerminator);
    Q_INVOKABLE void setWriteTermination(QString writeTerminator);
    Q_INVOKABLE void setBaudRate(BaudRateType baudRateType);
    Q_INVOKABLE void setDataBits(DataBitsType dataBitsType);
    Q_INVOKABLE void setStopBits(StopBitsType stopBitsType);
    Q_INVOKABLE void setParity(ParityType parityType);
    Q_INVOKABLE bool open(QIODevice::OpenMode mode);
    Q_INVOKABLE QString receive();
    Q_INVOKABLE bool dataAvailable();
    Q_INVOKABLE void setBaudRate(QString baudRateString);
    Q_INVOKABLE void setDataBits(QString dataBitsString);
    Q_INVOKABLE void setStopBits(QString stopBitsString);
    Q_INVOKABLE void setParity(QString parityString);

private:
    void processData(QByteArray data);
    void initialiseHashMaps();
    QextSerialPort *serialPort;
    QString writeTerminator;
    QString readTerminator;
    QString dataReceived;
    QHash <QString, BaudRateType> baudRate;
    QHash <QString, DataBitsType> dataBits;
    QHash <QString, StopBitsType> stopBits;
    QHash <QString, ParityType> parity;

signals:
    void sendString(QString string);
    void replyReady(QString dataReceived);

public slots:
   void onDataAvailable();
   Q_INVOKABLE void send(QString string);
};

#endif // SERIALPORT_H
