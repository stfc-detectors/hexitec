#include <QVariant>
#include "dataacquisitionfactory.h"
//#include "motor.h"
//#include "scriptingwidget.h"
#include "dataacquisitiondefinition.h"
#include "parameters.h"

DataAcquisitionFactory *DataAcquisitionFactory::dafInstance = 0;

DataAcquisitionFactory::DataAcquisitionFactory(DataAcquisitionForm *dataAcquisitionForm, DetectorControlForm *detectorControlForm,
                                               ProgressForm *progressForm, ProcessingBufferGenerator *processingBufferGenerator, QObject *parent)
{
   QString aspectFilename = Parameters::aspectIniFilename;
   QSettings *settings = new QSettings(QSettings::UserScope, "TEDDI", "HexitecGigE");

//   motorModel = new MotorModel();
//   motorFactory = MotorFactory::instance();
//   QString motor;
   int id = 0;
//   QList<QString> motors;
//   motors << "ssx" << "ssy" << "ssz" << "ssrot" << "galx" << "galy" << "galz" << "galrot";

//   for (int i = 0; i < motors.size(); ++i)
//   {
//      motor = motors.at(i);
//      if (settings->contains(motor))
//      {
//         /* The possibilities need to be properly shared with the configuration class. */
//         switch (settings->value(motor).toInt())
//         {
//         case 1:
//            if (settings->contains(motor+"Id"))
//            {
//               id = settings->value(motor+"Id").toInt();
//               motorFactory->createMotor(motor, GALIL, id);
//            }
//            else
//            {
//               // TO DO : replace with emits of writeMessage or some such...
//               //mcUi->daqOutput->append("Error creating Galil motor, " + motor + ", failed to get ID.");
//               qDebug() << "Error creating Galil motor, " << motor << ", failed to get ID.";
//            }
//            break;
//         case 2:
//            motorFactory->createMotor(motor, DUMMY);
//            break;
//         case 3:
//            if (settings->contains(motor+"Id"))
//            {
//               id = settings->value(motor+"Id").toInt();
//               motorFactory->createMotor(motor, NEWPORT, id);
//            }
//            else
//            {
//               // TO DO : replace with emits of writeMessage or some such...
//               //mcUi->daqOutput->append("Error creating Newport motor, " + motor + ", failed to get ID.");
//               qDebug() << "Error creating Newport motor, " << motor << ", failed to get ID.";
//            }
//            break;
//         default:
//            // TO DO : replace with emits of writeMessage or some such...
//            //mcUi->daqOutput->append("Error creating motor, " + motor + ", not found in settings.");
//            qDebug() << "Error creating motor, " << motor << ", not found in settings.";
//         }
//      }
//   }

//   motorFactory->createMotor("timer", DUMMY);

   // TO DO : Check whether a keithley is configured before the following.
   // If the configuration is done by setting a valid address this can also
   // be used to replace 24 below.
   voltageSourceFactory = VoltageSourceFactory::instance();
   voltageSourceFactory->createHV("hv");

   // TO DO : Check whether an aspect detector is configured before the following.
   /* Currently the following should be done after creating the keithley (above).
    * The DetectorMonitor created by the DetectorFactory uses the keithley!
    */

   if (settings->contains("aspectIniFilename"))
   {
      aspectFilename = settings->value("aspectIniFilename").toString();
   }
   detectorFactory = DetectorFactory::instance();
   detectorFactory->createGigEDetector(aspectFilename, parent);

   dataAcquisition = DataAcquisition::instance();
   dataAcquisition->setProperty("objectName", "daq");
   dataAcquisitionModel = DataAcquisitionModel::instance(dataAcquisitionForm, detectorControlForm, progressForm, processingBufferGenerator);
   dataAcquisitionModel->setProperty("objectName", "daqModel");

//   connect(this, SIGNAL(addObject(QObject*, bool, bool)), ScriptingWidget::instance()->getScriptRunner(),
//           SLOT(addObject(QObject*, bool, bool)));
   connect(this, SIGNAL(writeError(QString)), ApplicationOutput::instance(), SLOT(writeError(QString)));
   connect(this, SIGNAL(writeMessage(QString)), ApplicationOutput::instance(), SLOT(writeMessage(QString)));

   emit addObject(dataAcquisition, FALSE, TRUE);
   emit addObject(dataAcquisitionModel, FALSE, TRUE);

   try
   {
      detectorFactory->getGigEDetector()->initialiseConnection();
      detectorFactory->getGigEDetector()->beginMonitoring();
//      connectMotorsToDAQ();
   }
   catch (DetectorException &ex)
   {
//      connectMotorsToDAQ();
      detectorControlForm->initialiseFailed();
      emit writeError(ex.getMessage());
   }
}

//void DataAcquisitionFactory::connectMotorsToDAQ()
//{
//   Motor *motor;
//   QList <Motor *> motors;

//   motors = motorModel->getMotors();
//   QListIterator<Motor*> i(motors);

//   while (i.hasNext())
//   {
//      motor = i.next();
//      connect(motor, SIGNAL(positionChanged(Motor*,QVariant)),
//              dataAcquisition, SLOT(positionChanged(Motor*,QVariant)));
//      motor->positionChanged(motor, QVariant(motor->getPosition()));
//   }
//}

DataAcquisitionFactory::~DataAcquisitionFactory()
{
   dafInstance = 0;
   delete dataAcquisition;
   delete detectorFactory;
//   delete motorFactory;
}

DataAcquisitionFactory *DataAcquisitionFactory::instance(DataAcquisitionForm *dataAcquisitionForm, DetectorControlForm *detectorControlForm,
                                                         ProgressForm *progressForm, ProcessingBufferGenerator *processingBufferGenerator, QObject *parent)
{
   if (dafInstance == 0)
   {
      dafInstance = new DataAcquisitionFactory(dataAcquisitionForm, detectorControlForm, progressForm, processingBufferGenerator, parent);
   }

   return dafInstance;
}

DataAcquisitionFactory *DataAcquisitionFactory::getInstance()
{
   return dafInstance;
}

DataAcquisition *DataAcquisitionFactory::getDataAcquisition()
{
   return dataAcquisition;
}

DataAcquisitionModel *DataAcquisitionFactory::getDataAcquisitionModel()
{
   return dataAcquisitionModel;
}
