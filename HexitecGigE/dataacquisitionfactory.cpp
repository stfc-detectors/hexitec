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

   connect(this, SIGNAL(writeError(QString)), ApplicationOutput::instance(), SLOT(writeError(QString)));
   connect(this, SIGNAL(writeMessage(QString)), ApplicationOutput::instance(), SLOT(writeMessage(QString)));

   emit addObject(dataAcquisition, FALSE, TRUE);
   emit addObject(dataAcquisitionModel, FALSE, TRUE);

   try
   {
      detectorFactory->getGigEDetector()->initialiseConnection();
      detectorFactory->getGigEDetector()->beginMonitoring();
   }
   catch (DetectorException &ex)
   {
      detectorControlForm->initialiseFailed();
      emit writeError(ex.getMessage());
   }
}

DataAcquisitionFactory::~DataAcquisitionFactory()
{
   dafInstance = 0;
   delete dataAcquisition;
   delete detectorFactory;
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
