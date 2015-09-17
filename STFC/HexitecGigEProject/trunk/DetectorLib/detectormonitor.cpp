#include "detectormonitor.h"

#include <QDebug>
#include <QDateTime>
#include <cmath>

DetectorMonitor::DetectorMonitor(GigEDetector *gigEDetector, QObject *parent) :
   QObject(parent)
{
   monitoringEnabled = false;
   this->loggingInterval = gigEDetector->getLoggingInterval();
   monitorCount = this->loggingInterval;
   logfileWriter = NULL;
   readTAsic =false;
   temperatureInRange = false;
   a = 6.112;
   b = 17.67;
   c = 243.5;
   timer = new QTimer(this);
   connect(timer, SIGNAL(timeout()), this, SLOT(monitor()));
   this->gigEDetector = gigEDetector;
}

DetectorMonitor::~DetectorMonitor()
{
}

int DetectorMonitor::stop()
{
   int status = 0;

   return status;
}

int DetectorMonitor::getLoggingInterval()
{
   return loggingInterval;
}

int DetectorMonitor::start()
{
   int status = 0;

   timer->start(1000);

   return status;
}

void DetectorMonitor::monitor()
{
   if (monitoringEnabled)
   {
      monitorEnvironmentalValues();
   }
}

void DetectorMonitor::executeMonitorEnvironmentalValues()
{
   if (!monitoringEnabled)
   {
      monitorEnvironmentalValues();
   }
}

void DetectorMonitor::monitorEnvironmentalValues()
{
   read();
   emit updateMonitorData(new MonitorData(th, t, tdp, rh, ik, tasic));
   emit monitoringDone();

   if (logfileWriter != NULL)
   {
      if (monitorCount == loggingInterval)
      {
         logfileWriter->append(QDateTime::currentDateTime().toString("yyMMdd_hhmmss") + " " +
                               QString::number(t, 'f', 1) + " " +
                               QString::number(th, 'f', 1) + " " +
                               QString::number(tdp, 'f', 1) + " " +
                               QString::number(tasic, 'f', 1) + " " +
                               QString::number(tdac, 'f', 1) + " " +
                               QString::number(rh, 'f', 1));
         monitorCount = 0;
      }
      monitorCount++;
   }

   if (t < tdp)
   {
      if (temperatureInRange)
      {
         emit temperatureBelowDP();
         temperatureInRange = false;
      }
   }
   else if (!temperatureInRange)
   {
      emit temperatureAboveDP();
      temperatureInRange = true;
   }

}

void DetectorMonitor::read()
{
   int status = -1;

   status = gigEDetector->getDetectorValues(&rh, &th, &tasic, &tdac, &t, &ik);

   if(!status)
   {
       calcTDP();
   }
}

void DetectorMonitor::calcTDP()
{
   gamma = log(rh/100) + ((b*th) / (c+th));
   tdp = c * gamma / (b - gamma);
}

void DetectorMonitor::enableMonitoring()
{
   monitoringEnabled = true;
}

void DetectorMonitor::disableMonitoring()
{
   monitoringEnabled = false;
}

void DetectorMonitor::handleWriteError(QString message)
{
   emit writeError(message);
}

void DetectorMonitor::handleWriteMessage(QString message)
{
   emit writeMessage(message);
}

void DetectorMonitor::handleBiasRefreshing()
{
   readTAsic = true;
}

void DetectorMonitor::receiveState(GigEDetector::DetectorState detectorState)
{
   if (detectorState == GigEDetector::INITIALISED)
   {
      readTAsic = true;
      read();
   }
}

void DetectorMonitor::createLogFile(DetectorFilename *logFilename)
{
   QString filename;

   if (logfileWriter != NULL)
   {
      delete logfileWriter;
   }

   if (logFilename == NULL)
   {
      logfileWriter = NULL;
      monitorCount = loggingInterval;
   }
   else
   {
      filename.append(logFilename->getPrefix());
      logfileWriter = new FileWriter(logFilename->getDirectory(), filename, logFilename->getTimestampOn(), true);
      if (logfileWriter != NULL)
      {
         logfileWriter->append("Timestamp : Finger Temp : Housing Temp : Dew Point : Detector Temp : DAC Temp : Relative Humidity");
      }
   }

}
