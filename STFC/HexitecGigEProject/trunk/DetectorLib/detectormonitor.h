#ifndef DETECTORMONITOR_H
#define DETECTORMONITOR_H

/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX

#include <QObject>
#include <QTimer>

#include "gigedetector.h"
#include "monitordata.h"
#include "detectorfilename.h"
#include "filewriter.h"
#include <Windows.h>

#define HEXITEC_MONITORING_DONE TEXT("Hexitec_MonitoringDone")
#define HEXITEC_TEMPERATURE_BELOWDP TEXT("Hexitec_TemperatureBelowDP")

class DetectorMonitor : public QObject
{
   Q_OBJECT
private:
   QTimer *timer;
   GigEDetector *gigEDetector;
   double th, t, tdp, rh, ik, tasic, tdac;
   double a, b, c;
   double gamma;
   bool valid;
   bool temperatureInRange;
   bool readTAsic;
   int loggingInterval;
   int monitorCount;
   bool monitoringEnabled;
   FileWriter *logfileWriter;
   HANDLE monitoringDoneEvent;
   HANDLE temperatureBelowDPEvent;
   MonitorData *monitorData;
   void calcTDP();
   void read();
   void monitorEnvironmentalValues(bool external = false);


public:
   explicit DetectorMonitor(GigEDetector *gigEDetector, QObject *parent = 0);
   ~DetectorMonitor();
   int start();
   int stop();
   int getLoggingInterval();
   HANDLE *getTemperatureBelowDPEvent();
   HANDLE *getMonitoringDoneEvent();

signals:
   void updateMonitorData(MonitorData *md);
   void monitoringDone();
   void writeError(QString message);
   void writeMessage(QString message);
   void temperatureBelowDP();
   void temperatureAboveDP();

private slots:
   //void monitor();
   void handleWriteError(QString message);
   void handleWriteMessage(QString message);

public slots:
   void handleBiasRefreshing();
   void receiveState(GigEDetector::DetectorState detectorState);
   void createLogFile(DetectorFilename *logFilename);
   void executeMonitorEnvironmentalValues();
   void enableMonitoring();
   void disableMonitoring();
   MonitorData *monitor(bool external = false);
};

#endif // DETECTORMONITOR_H
