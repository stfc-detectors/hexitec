#include <QDebug>
#include <QIODevice>
#include <QStringList>
#include "serialport.h"

SerialPort::SerialPort(QString port, QObject *parent) :
    QObject(parent)
{
    serialPort = new QSerialPort(port);
    initialiseHashMaps();
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
}


void SerialPort::initialiseHashMaps()
{
    baudRate["1200"] = QSerialPort::Baud1200;
    baudRate["2400"] = QSerialPort::Baud2400;
    baudRate["9600"] = QSerialPort::Baud9600;
    baudRate["19200"] = QSerialPort::Baud19200;
    baudRate["38400"] = QSerialPort::Baud38400;
    baudRate["57600"] = QSerialPort::Baud57600;

    dataBits["5"] = QSerialPort::Data5;
    dataBits["6"] = QSerialPort::Data6;
    dataBits["7"] = QSerialPort::Data7;
    dataBits["8"] = QSerialPort::Data8;

    parity["Space Parity"] = QSerialPort::SpaceParity;
    parity["Mark Parity"] = QSerialPort::MarkParity;
    parity["No Parity"] = QSerialPort::NoParity;
    parity["Even Parity"] = QSerialPort::EvenParity;
    parity["Odd Parity"] = QSerialPort::OddParity;

    stopBits["1"] = QSerialPort::OneStop;
    stopBits["1.5"] = QSerialPort::OneAndHalfStop;
    stopBits["2"] = QSerialPort::TwoStop;

}


void SerialPort::send(QString string)
{
    QByteArray data;

    string.append(writeTermination());
    data = string.toLatin1();
    serialPort->write(data);
}

QString SerialPort::writeTermination()
{
    return writeTerminator;
}

QString SerialPort::readTermination()
{
    return readTerminator;
}

void SerialPort::setReadTermination(QString readTerminator)
{
    this->readTerminator = readTerminator;
}

void SerialPort::setWriteTermination(QString writeTerminator)
{
    this->writeTerminator = writeTerminator;
}

void SerialPort::setBaudRate(QString baudRateString)
{
    setBaudRate(baudRate[baudRateString]);
}

void SerialPort::setBaudRate(QSerialPort::BaudRate baudRate)
{
    serialPort->setBaudRate(baudRate);
}

void SerialPort::setDataBits(QString dataBitsString)
{
    setDataBits(dataBits[dataBitsString]);
}

void SerialPort::setDataBits(QSerialPort::DataBits dataBits)
{
    serialPort->setDataBits(dataBits);
}

void SerialPort::setStopBits(QString stopBitsString)
{
    setStopBits(stopBits[stopBitsString]);
}

void SerialPort::setStopBits(QSerialPort::StopBits stopBits)
{
    serialPort->setStopBits(stopBits);
}

void SerialPort::setParity(QString parityString)
{
    setParity(parity[parityString]);
}

void SerialPort::setParity(QSerialPort::Parity parity)
{
    serialPort->setParity(parity);
}

bool SerialPort::open(QIODevice::OpenMode mode)
{
    return serialPort->open(mode);
}

void SerialPort::processData(QByteArray data)
{
    QString fragment = QString(data);

    dataReceived += fragment;
}

QString SerialPort::receive()
{
    QString reply = dataReceived;

    dataReceived.remove(0, reply.length());
    return reply;
}

bool SerialPort::dataAvailable()
{
    return dataReceived.length();
}

void SerialPort::onDataAvailable()
{
    QByteArray data;

    data = serialPort->readAll();
    processData(data);
}
