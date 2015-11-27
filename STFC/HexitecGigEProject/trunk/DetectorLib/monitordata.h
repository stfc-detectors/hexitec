#ifndef MONITORDATA_H
#define MONITORDATA_H

#include <QObject>

class MonitorData : public QObject
{
   Q_OBJECT
private:
   double th;
   double t;
   double tdp;
   double rh;
   double ik;
   double tasic;
   bool valid;
public:
   explicit MonitorData(double th, double t, double tdp, double rh, double ik, double tasic,
                        bool valid, QObject *parent = 0);
   double getTH();
   double getT();
   double getTDP();
   double getRH();
   double getIK();
   double getTASIC();
   bool getValid();
};

#endif // MONITORDATA_H
