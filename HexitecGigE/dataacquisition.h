#ifndef DATAACQUISITION_H
#define DATAACQUISITION_H

/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX

#include <QObject>
#include <QThread>
#include <QHash>
//#include "motor.h"
#include "gigedetector.h"
#include "hv.h"
#include "dataacquisitionmodel.h"
#include "dataacquisitiondefinition.h"
#include "dataacquisitionstatus.h"
#include "reservable.h"

class DataAcquisition : public QThread, public Reservable
{
   Q_OBJECT

public:
   static DataAcquisition *instance();
   ~DataAcquisition();
   Q_INVOKABLE bool isBusy();
   Q_INVOKABLE char *getStatus();
   Q_INVOKABLE void collectReducedImages();
protected:
   explicit DataAcquisition(QObject *parent = 0);
private:
   static DataAcquisition *daqInstance;
//   static double motorPosition;
   static void initHexitechProcessor();
//   static QHash<QString, int> motorPositions;
   void run();
   int waitForConfiguringDone();
   int waitForCollectingDone();
   void waitForMonitoringDone();
   int waitForBiasRefreshDone();
   int waitForTrigger();
   void configureTriggering(int triggeringMode);
   void configureDataCollection();
   void configureBasicCollection();
   void setDirectory(int repeatCount);
   void performContinuousDataCollection(bool triggering = false);
   void performTriggeredDataCollection();
   void performFixedDataCollection();
   int doSplitDataCollections(int nDaqOverall, int repeatCount,
                              bool triggering = false, int ttlInput = GigEDetector::INVALID_TTLINPUT);
   int doLowPriorityBiasDataCollections(int nDaqOverall);
   void setDataAcquisitionTime(int nDaq);
   void performSingleBiasRefresh();
   void performMonitorEnvironmentalValues();
   void performGigEDefaultDataCollection();
   void performTriggeringConfigure();
   void setAbort(bool abort);
   bool abortRequired();
   void pauseDataAcquisition();
   bool repeatPauseRequired(int repeatCount);
   void updateImageFile(int writeToFileNumber);
   void changeDAQStatus(DataAcquisitionStatus::MajorStatus majorStatus,
                        DataAcquisitionStatus::MinorStatus minorStatus);
   DataAcquisitionModel *dataAcquisitionModel;
   DataAcquisitionDefinition *dataAcquisitionDefinition;
   GigEDetector *gigEDetector;
   HV *hv;
   GigEDetector::Mode mode;
   int splitDataCollections;
   int nDaq;
   int nRepeat;
   int currentImageNumber;
   bool collecting;
   bool triggered;
   bool configuring;
   bool biasPriority;
   bool biasRefreshing;
   bool monitoring;
   bool biasOn;
   bool biasRefreshRequired;
   bool appendRepeatCount;
   bool abort;
   GigEDetector::DetectorState detectorState;
   double tdp;
   DataAcquisitionStatus daqStatus;
   DataAcquisitionStatus::MajorStatus storedMajorStatus;
   DataAcquisitionStatus::MinorStatus storedMinorStatus;
   double dataCollectionTime;
   bool busy;
   QList <QObject *> rdaql;
   Reservation reservation;
   unsigned long long totalFramesAcquired;
   unsigned long long totalImageFrames = -1;

signals:
   void executeCommand(GigEDetector::DetectorCommand, int, int);
   void executeOffsets();
   void executeReducedDataCollection();
   void storeBiasSettings();
   void restoreBiasSettings();
   void disableBiasRefresh();
   void enableBiasRefresh();
   void enableMonitoring();
   void disableMonitoring();
   void executeSingleBiasRefresh();
   void executeMonitorEnvironmentalValues();
   void pushFilename();
   void collectingChanged(bool);
   void dataAcquisitionStatusChanged(DataAcquisitionStatus dataAcquisitionStatus);
   void setTargetTemperature(double targetTemperature);
   void appendTimestamp(bool appendTimestamp);
   void imageComplete(unsigned long long totalFramesAcquired);
   void imageComplete(long long totalFramesAcquired);
   void imageStarting(double daqCollectionDuration, int repeatCount, int nRepeat);
   void transferBufferReady(unsigned char *transferBuffer, unsigned long validFrames);

public slots:
   void handleAbortDAQ();
   void receiveState(GigEDetector::DetectorState detectorState);
   void handleTriggeringSelectionChanged(int triggering);
   void handleTtlInputSelectionChanged(int ttlInput);
   void handleCollectReducedImages();
   void handleCollectFixedImages();
   void handleExecuteOffsets();
   void handleCancelOffsets();
   void handleExecuteReducedDataCollection();
   void handleCancelReducedDataCollection();
   void handleBiasRefreshing();
   void handleBiasRefreshed(QString time, bool restartMonitoring);
   void handleMonitored();
   void handleBiasState(bool biasOn);
   void handleSetFingerTemperature(double temperature);
   void handleMonitorData(MonitorData *md);
//   void positionChanged(Motor *motor, const QVariant & value);
   void handleBufferReady(unsigned char * transferBuffer, unsigned long validFrames);
//   void handleImageStarted(char *path, int frameSize);
   void handleImageComplete(unsigned long long framesAcquired);
   void handleInitialiseDetector();
   void prepareForBiasRefresh();

private slots:
   //void handlePushFilename();
};

#endif // DATAACQUISITION_H
