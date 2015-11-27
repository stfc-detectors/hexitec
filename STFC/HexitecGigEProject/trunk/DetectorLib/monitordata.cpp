#include "monitordata.h"

MonitorData::MonitorData(double th, double t, double tdp, double rh, double ik, double tasic,
                         bool valid, QObject *parent) :
   QObject(parent)
{
   this->th = th;
   this->t = t;
   this->tdp = tdp;
   this->rh = rh;
   this->ik = ik;
   this->tasic = tasic;
   this->valid = valid;
}

double MonitorData::getTH()
{
   return th;
}

double MonitorData::getT()
{
   return t;
}

double MonitorData::getTDP()
{
   return tdp;
}

double MonitorData::getRH()
{
   return rh;
}

double MonitorData::getIK()
{
   return ik;
}

double MonitorData::getTASIC()
{
   return tasic;
}

bool MonitorData::getValid()
{
   return valid;
}
