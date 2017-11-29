#ifndef DATAACQUISITIONMODEL_H
#define DATAACQUISITIONMODEL_H

/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX

#include <QObject>
#include "detectorcontrolform.h"
#include "dataacquisitionform.h"
#include "processingbuffergenerator.h"
#include "progressform.h"
#include "gigedetector.h"
#include "detectormonitor.h"
#include "hv.h"
#include "dataacquisitiondefinition.h"
#include "detectorfilename.h"
#include "objectreserver.h"
#include "reservable.h"
#include "applicationoutput.h"

// Putting the .h files for these classes here can introduce circularity problems so
// we just declare the classes and put the .h files in the .cpp file.

class DataAcquisition;

class DataAcquisitionModel : public QObject, public Reservable
{
   Q_OBJECT

private:
   explicit DataAcquisitionModel(DataAcquisitionForm *dataAcquisitionForm,
                                 DetectorControlForm *detectorControlForm,
                                 ProgressForm *progressForm,
                                 ProcessingBufferGenerator *processingBufferGenerator,
                                 QObject *parent = 0);
   static DataAcquisitionModel *damInstance;
   DataAcquisitionForm *dataAcquisitionForm;
   DetectorControlForm *detectorControlForm;
   ProgressForm *progressForm;
   ProcessingBufferGenerator *processingBufferGenerator;
   HV *hv;
   GigEDetector *gigEDetector;
   DetectorMonitor *detectorMonitor;
   DataAcquisition *dataAcquisition;
   DataAcquisitionDefinition dataAcquisitionDefinition;
   ObjectReserver *objectReserver;
   double daqCollectionDuration;
   double daqDuration;
   QList <QObject *> rdaqml;

   void connectDetectorMonitor();
   void connectDataAcquisition();
   void connectGigEDetector();
   void connectDataAcquisitionForm();
   void connectDetectorControlForm();
   void connectDataAcquisitionModel();
   void connectHV();
   void connectObjectReserver();
   void setDetectorFilename(DetectorFilename sourceFilename, DetectorFilename *destFilename);
   void initialise();
   void initialiseDetectorFilename(DetectorFilename *detectorFilename);
   void changeDaqDuration();

public:
   static DataAcquisitionModel *instance(DataAcquisitionForm *dataAcquisitionForm, DetectorControlForm *detectorControlForm,
                                         ProgressForm *progressForm, ProcessingBufferGenerator *processingBufferGenerator, QObject *parent = 0);
   static DataAcquisitionModel *getInstance();
   ~DataAcquisitionModel();
   DataAcquisitionDefinition *getDataAcquisitionDefinition();
   Q_INVOKABLE void configure(QString directory, QString prefix, bool timestampOn, bool offsets, double duration, int repeatCount, int repeatInterval, bool triggering, int ttlInput);
   Q_INVOKABLE void setDirectory(QString directory);
   Q_INVOKABLE void setPrefix(QString prefix);
   Q_INVOKABLE void setTimestampOn(bool timestampOn);
   Q_INVOKABLE QString getDirectory();
   QList<QObject *> getReserveList();
   double getDaqCollectionDuration();
   double getDaqDuration();


signals:
   void dataChanged(DataAcquisitionDefinition dataAcquisitionDefinition);
   void dataChanged(QString mode);
   void daqDurationChanged(double daqDurationTime);
   void biasVoltageChanged(bool);
   void triggeringChanged(bool);

public slots:
   void handleDataFilenameChanged(DetectorFilename dataFilename);
   void handleLogFilenameChanged(DetectorFilename logFilename);
   void handleDataAcquisitionDefinitionChanged(DataAcquisitionDefinition dataAcquisitionDefinition);
   void handleFixedImageCountChanged(int fixedImageCount);
   void handleBiasVoltageChanged(bool biasOn);
   void handleTriggeringChanged(int triggering);
};

#endif // DATAACQUISITIONMODEL_H
