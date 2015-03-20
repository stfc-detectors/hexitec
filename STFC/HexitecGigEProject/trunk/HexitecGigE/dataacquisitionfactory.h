#ifndef DATAACQUISITIONFACTORY_H
#define DATAACQUISITIONFACTORY_H

#include <QObject>
#include "motorfactory.h"
#include "detectorfactory.h"
#include "dataacquisition.h"
#include "voltageSourceFactory.h"
#include "dataacquisitionmodel.h"
#include "ui_motioncontrolform.h"
#include "ui_dataacquisitionform.h"
#include "ui_detectorcontrolform.h"

class DataAcquisitionFactory : public QObject
{
   Q_OBJECT
public:
   ~DataAcquisitionFactory();
   static DataAcquisitionFactory *instance(DataAcquisitionForm *dataAcquisitionForm, DetectorControlForm *detectorControlForm);
   static DataAcquisitionFactory *getInstance();
   DataAcquisition *getDataAcquisition();
   DataAcquisitionModel *getDataAcquisitionModel();
protected:
   DataAcquisitionFactory(DataAcquisitionForm *dataAcquisitionForm, DetectorControlForm *detectorControlForm,
                          QObject *parent = 0);
private:
   MotorModel *motorModel;
   MotorFactory *motorFactory;
   DetectorFactory *detectorFactory;
   VoltageSourceFactory *voltageSourceFactory;
   DataAcquisition *dataAcquisition;
   DataAcquisitionModel *dataAcquisitionModel;
   static DataAcquisitionFactory *dafInstance;
   void connectMotorsToDAQ();

signals:
   void addObject(QObject *object, bool scripting = TRUE, bool gui = TRUE);
};

#endif // DATAACQUISITIONFACTORY_H