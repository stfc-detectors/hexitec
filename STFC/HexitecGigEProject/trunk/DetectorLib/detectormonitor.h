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
//#include "filewriter.h"

class DetectorMonitor : public QObject
{
   Q_OBJECT
private:
   QTimer *timer;
   GigEDetector *gigEDetector;
//   Keithley *keithley;
   double th, t, tdp, rh, ik, tasic;
   double a, b, c;
   double gamma;
   bool temperatureInRange;
   bool readTAsic;
   int loggingInterval;
   int monitorCount;
//   FileWriter *logfileWriter;
   void calcTDP();
   void read();

public:
   explicit DetectorMonitor(GigEDetector *gigEDetector, int loggingInterval, QObject *parent = 0);
   ~DetectorMonitor();
   int start();
   int stop();
   int getLoggingInterval();
   
signals:
   void updateMonitorData(MonitorData *md);
   void writeError(QString message);
   void writeMessage(QString message);
   void temperatureBelowDP();
   void temperatureAboveDP();

private slots:
   void monitor();
   void handleWriteError(QString message);
   void handleWriteMessage(QString message);

public slots:
   void handleBiasRefreshing();
   void receiveState(GigEDetector::DetectorState detectorState);
   void createLogFile(DetectorFilename *logFilename);
};

#endif // DETECTORMONITOR_H