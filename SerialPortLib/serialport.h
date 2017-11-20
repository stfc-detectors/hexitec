#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QHash>
#include <QSerialPort>

class SerialPort : public QObject
{
    Q_OBJECT

public:
    explicit SerialPort(QString port, QObject *parent = 0);
    Q_INVOKABLE QString writeTermination();
    Q_INVOKABLE QString readTermination();
    Q_INVOKABLE void setReadTermination(QString readTerminator);
    Q_INVOKABLE void setWriteTermination(QString writeTerminator);
    Q_INVOKABLE void setBaudRate(QSerialPort::BaudRate baudRate);
    Q_INVOKABLE void setDataBits(QSerialPort::DataBits dataBits);
    Q_INVOKABLE void setStopBits(QSerialPort::StopBits stopBits);
    Q_INVOKABLE void setParity(QSerialPort::Parity parity);
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
    QSerialPort *serialPort;
    QString writeTerminator;
    QString readTerminator;
    QString dataReceived;

    QHash <QString, QSerialPort::BaudRate> baudRate;
    QHash <QString, QSerialPort::DataBits> dataBits;
    QHash <QString, QSerialPort::StopBits> stopBits;
    QHash <QString, QSerialPort::Parity> parity;


signals:
    void sendString(QString string);
    void replyReady(QString dataReceived);

public slots:
   void onDataAvailable();
   Q_INVOKABLE void send(QString string);
};

#endif // SERIALPORT_H
