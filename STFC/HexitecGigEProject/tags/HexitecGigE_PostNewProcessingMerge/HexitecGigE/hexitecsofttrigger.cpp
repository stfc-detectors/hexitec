#include "hexitecsofttrigger.h"
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <cstdlib>
#include <sstream>

using namespace std;

HexitecSoftTrigger::HexitecSoftTrigger(QObject *parent) :
    QThread(parent)
{

    EOLcode = "\n";
    // default file names and locations
    commandFileName = "Command.txt";
    commandFile = new QFile(commandFileName);
    statusFileName = "Status.txt";
    statusFile = new QFile("galilStatus.txt");
    dataFileLocations = "U:\\HexitecImages\\"; //should we have the slash slash at the end
    dataFilePrefix = "TEST";
    countTimeMS = 5000;
    fileNumber = 0;
    lastFileWritten = "";
    liveRead = false;
    pollTime = 250;
    setNextFileName();
    sleepState = false;
    sleepTime = 0;

    // setup up the timers
    writeTimer = new QTimer(this);
    connect(writeTimer, SIGNAL(timeout()), this, SLOT(writeCommandFile()));
    connect(this, SIGNAL(startWriteTimerSignal()), this, SLOT(startWriteTimer()));
    connect(this, SIGNAL(stopWriteTimerSignal()), this, SLOT(stopWriteTimer()));
    readTimer = new QTimer(this);
    connect(readTimer, SIGNAL(timeout()), this, SLOT(readStatusFile()));
    connect(this, SIGNAL(startReadTimerSignal()), this, SLOT(startReadTimer()));
    connect(this, SIGNAL(stopReadTimerSignal()), this, SLOT(stopReadTimer()));
    sleepTimer = new QTimer(this);
    connect(sleepTimer, SIGNAL(timeout()), this, SLOT(sleepEnd()));
    connect(this, SIGNAL(startSleepTimerSignal()), this, SLOT(startSleepTimer()));
    connect(this, SIGNAL(stopSleepTimerSignal()), this, SLOT(stopSleepTimer()));
  }

void HexitecSoftTrigger::startWriteTimer()
{
   writeTimer->start(1000);
}

void HexitecSoftTrigger::stopWriteTimer()
{
   writeTimer->stop();
}

void HexitecSoftTrigger::startReadTimer()
{
   readTimer->start(pollTime);
}

void HexitecSoftTrigger::stopReadTimer()
{
   readTimer->stop();
}

void HexitecSoftTrigger::startSleepTimer()
{
   sleepTimer->start(sleepTime);
}

void HexitecSoftTrigger::stopSleepTimer()
{
   sleepTimer->stop();
}

void HexitecSoftTrigger::sleep(int localSleepTime)
{
    if (sleepTimer->isActive());
        emit stopSleepTimerSignal();
    if (localSleepTime == 0)
        return;
    sleepTime = localSleepTime;
    sleepState = true;
    writeMessage("sleepState = " + QString::number(sleepState));
    emit startSleepTimerSignal();
}

void HexitecSoftTrigger::sleepEnd()
{
    emit stopSleepTimerSignal();
    sleepState = false;
    writeMessage("sleepState = " + QString::number(sleepState));
}

void HexitecSoftTrigger::setNextFileName()
{
    nextFileToWrite.sprintf("%5.5d_" ,fileNumber);
    nextFileToWrite = dataFilePrefix + nextFileToWrite;
}

void HexitecSoftTrigger::acquire()
{
    if (!commandFile->isOpen())
        commandFile->open(QIODevice::WriteOnly | QIODevice::Text );
    if (!statusFile->isOpen())
        statusFile->open(QIODevice::ReadOnly | QIODevice::Text );
    if (!commandFile->isOpen())
    {
        emit writeError("Failed to open Hexitec Command.txt");
        return;
    }
    if (!statusFile->isOpen())
    {
        emit writeError("Failed to open Hexitec Status.txt");
        if (readTimer->isActive())
            emit stopReadTimerSignal();
        return;
    }
    state = false;
    writeCommandFile();
    state = true;
    //QTimer::singleShot(500, this , SLOT(writeCommandFile(true)));

   emit startWriteTimerSignal();
}

void HexitecSoftTrigger::acquire(unsigned int countTime)
{
    countTimeMS = countTime * 1000;
    acquire();
}

void HexitecSoftTrigger::writeCommandFile()
{

    if (readTimer->isActive())
    {
        writeError("busy checking Status.txt; cannot acquire");
        emit stopWriteTimerSignal();
        //writeTimer->stop();
        return;
    }

    QTextStream out(commandFile);
    commandFile->reset();
    if (verbose)
        emit writeMessage("writing " + commandFileName);
    if (state)
         out << "1" << EOLcode;
    else
        out << "0" << EOLcode;
    out << countTimeMS << EOLcode;
    out << dataFileLocations << EOLcode;
    out << nextFileToWrite << EOLcode;
    out << EOLcode;
    out.flush();
    if (state)
    {
        emit stopWriteTimerSignal();
        //writeTimer->stop();
        //timer->start(pollTime);
        emit startReadTimerSignal();
        //state = false;
        //commandFile->close();
        //statusFile->close();
    }
}

void HexitecSoftTrigger::readStatusFile()
{

    emit writeMessage("Checking status.txt...");
    statusFile->reset();
    QTextStream in(statusFile);
    int intStat = 1;
    int measureTime = 0;
    in >> intStat;
    in >> measureTime;
    double fractionComplete = measureTime / countTimeMS;

    if (intStat == 1)
    {
        if (verbose)
            emit writeMessage("Recording: " + QString::number(intStat) + " Time: "+ QString::number(measureTime) + " Fraction " + QString::number(fractionComplete));
    }
    if (intStat == 0 && measureTime == 0)
    {
        state = 0;
        emit stopReadTimerSignal();
        //timer->stop();
        lastFileWritten = dataFileLocations + nextFileToWrite;

// Uncomment the next two lines when Aspect fix their code
        //fileNumber++;
        //setNextFileName();
        if (liveRead)
        {
            emit readLastAcquiredFile(lastFileWritten+"reduced_00000.dat");
        }
// Delete the next two lines when Aspect fix their shitty code
        fileNumber++;
        setNextFileName();
    }
//    if (intStat == 1)
//        statusFile->close();

}

void HexitecSoftTrigger::kill()
{
    QMutexLocker locker(&mutex);
    if (readTimer->isActive())
        emit stopReadTimerSignal();
}

void HexitecSoftTrigger::currentSettings()
{
    QMutexLocker locker(&mutex);
    QString Message;
    Message = "<B>HEXITEC acquisition information</B>";
    //   Message+= "ol";
    Message+= "<li>Status file : " + statusFileName + "</li>";
    Message+= "<li>Command file : " + commandFileName + "</li>";
    Message+= "<li>Data file locations: " + dataFileLocations + "</li>";
    Message+= "<li>Count Time (milliseconds): " + QString::number(countTimeMS) + "</li>";
    Message+= "<li>filePrefix : " + dataFilePrefix + "</li>";
    Message+= "<li>fileNumber : " + QString::number(fileNumber) + "</li>";
    Message+= "<li>Last file written : " + lastFileWritten + "</li>";
    Message+= "<li>Next filename : " + nextFileToWrite + "</li>";
    //   Message+= "/ol";
    emit writeMessage(Message);
}

/*
// write the Command.txt file with 0 status
ofstream commandFile;
commandFile.open(commandFileName.toAscii());
commandFile << 0 << endl;
//    commandFile << QString::number(countTimeMS).toStdString() << endl;
//    commandFile << dataFileLocations.toStdString() << endl;
//    commandFile << nextFileToWrite.toStdString() << endl;
commandFile.close();
// write the Command.txt file with 1 status
commandFile.open(commandFileName.toAscii());
commandFile << 1 << endl;
commandFile << QString::number(countTimeMS).toStdString() << endl;
commandFile << dataFileLocations.toStdString() << endl;
commandFile << nextFileToWrite.toStdString() << endl;
commandFile.close();
*/

/*
   ifstream statusFile;
   int intStat = 1;
   bool ctrlEventFlag = false;
   int measureTime = 0;
   statusFile.open(statusFileName.toAscii());
   statusFile >> intStat;
   statusFile >> measureTime;
   writeMessage("Status: " + QString::number(intStat) + " Time: "+ QString::number(measureTime));
   statusFile.close();
   */
