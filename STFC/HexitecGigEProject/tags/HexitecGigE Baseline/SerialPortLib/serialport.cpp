#include <QDebug>
#include <QIODevice>
#include <QStringList>
#include "serialport.h"
#include "windows.h"

SerialPort::SerialPort(QString port, QObject *parent) :
    QObject(parent)
{
    serialPort = new QextSerialPort(port);
    initialiseHashMaps();
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
}

void SerialPort::initialiseHashMaps()
{
    baudRate["110"] = BAUD110;
    baudRate["300"] = BAUD300;
    baudRate["600"] = BAUD600;
    baudRate["1200"] = BAUD1200;
    baudRate["2400"] = BAUD2400;
    baudRate["9600"] = BAUD9600;
    baudRate["14400"] = BAUD14400;
    baudRate["19200"] = BAUD19200;
    baudRate["38400"] = BAUD38400;
    baudRate["56000"] = BAUD56000;
    baudRate["57600"] = BAUD57600;
    baudRate["128000"] = BAUD128000;
    baudRate["256000"] = BAUD256000;

    dataBits["5"] = DATA_5;
    dataBits["6"] = DATA_6;
    dataBits["7"] = DATA_7;
    dataBits["8"] = DATA_8;

    parity["Space Parity"] = PAR_SPACE;
    parity["Mark Parity"] = PAR_MARK;
    parity["No Parity"] = PAR_NONE;
    parity["Even Parity"] = PAR_EVEN;
    parity["Odd Parity"] = PAR_ODD;

    stopBits["1"] = STOP_1;
    stopBits["1.5"] = STOP_1_5;
    stopBits["2"] = STOP_2;
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

void SerialPort::setBaudRate(BaudRateType baudRateType)
{
    serialPort->setBaudRate(baudRateType);
}

void SerialPort::setDataBits(QString dataBitsString)
{
    setDataBits(dataBits[dataBitsString]);
}

void SerialPort::setDataBits(DataBitsType dataBitsType)
{
    serialPort->setDataBits(dataBitsType);
}

void SerialPort::setStopBits(QString stopBitsString)
{
    setStopBits(stopBits[stopBitsString]);
}

void SerialPort::setStopBits(StopBitsType stopBitsType)
{
    serialPort->setStopBits(stopBitsType);
}

void SerialPort::setParity(QString parityString)
{
    setParity(parity[parityString]);
}

void SerialPort::setParity(ParityType parityType)
{
    serialPort->setParity(parityType);
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
