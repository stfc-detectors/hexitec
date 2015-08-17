#ifndef DATAACQUISITION_H
#define DATAACQUISITION_H

/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX

#include <QObject>
#include <QThread>
#include <QHash>
#include "motor.h"
#include "gigedetector.h"
#include "keithley.h"
#include "dataacquisitionmodel.h"
#include "dataacquisitiondefinition.h"
#include "hxtprocessing.h"
#include "filewriter.h"
#include "dataacquisitionstatus.h"
#include "reservable.h"

class DataAcquisition : public QThread, public Reservable
{
   Q_OBJECT
/*
   static LONG __cdecl fileCallback(CONST LPSTR path)
   {
      LONG status = 0;

      //qDebug() << "\n================== In DataAcquisition callback" << path << " AND mTime = " << mPos->mTime;
      hxtProcessor->pushRawFileName(path);

      hxtProcessor->pushMotorPositions(&motorPositions);

      return status;
   }
*/
public:
   static DataAcquisition *instance();
   ~DataAcquisition();
   Q_INVOKABLE bool isBusy();
   Q_INVOKABLE char *getStatus();
   Q_INVOKABLE bool isWaitingForTrigger();
   Q_INVOKABLE bool isCollectingTriggered();
   Q_INVOKABLE void collectReducedImages();
   Q_INVOKABLE void initTrigger();
   Q_INVOKABLE void trigger();
   Q_INVOKABLE void stopTrigger();
protected:
   explicit DataAcquisition(QObject *parent = 0);
private:
   static DataAcquisition *daqInstance;
   static hexitech::HxtProcessing *hxtProcessor;
   static double motorPosition;
   static void initHexitechProcessor();
   static QHash<QString, int> motorPositions;
   void run();
   int waitForCollectingDone();
   int waitForBiasRefreshDone();
   void configureDataCollection();
   void setDirectory(int repeatCount);
   void initialiseTriggeredDataCollection();
   void performContinuousDataCollection();
   void performTriggeredDataCollection();
   void performFixedDataCollection();
   void setDataAcquisitionTime(int nDaq);
   void performSingleBiasRefresh();
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
   FileWriter *imageIndicatorFile;
   Keithley *keithley;
   GigEDetector::Mode mode;
   int splitDataCollections;
   int nDaq;
   int nRepeat;
   int currentImageNumber;
   bool collecting;
   bool biasRefreshing;
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
   bool waitingForTrigger;
   bool collectingTriggered;
   QList <QObject *> rdaql;
   Reservation reservation;

   void performGigEDefaultDataCollection();
signals:
   void executeCommand(GigEDetector::DetectorCommand, int, int);
   void executeOffsets();
   void executeReducedDataCollection();
   void storeBiasSettings();
   void restoreBiasSettings();
   void disableBiasRefresh();
   void executeSingleBiasRefresh();
   void pushFilename();
   void collectingChanged(bool);
   void dataAcquisitionStatusChanged(DataAcquisitionStatus dataAcquisitionStatus);
public slots:
   void handleAbortDAQ();
   void handleModeChanged(GigEDetector::Mode mode);
   void receiveState(GigEDetector::DetectorState detectorState);
   void handleCollectReducedImages();
   void handleCollectFixedImages();
   void handleInitTrigger();
   void handleTrigger();
   void handleStopTrigger();
   void handleExecuteOffsets();
   void handleCancelOffsets();
   void handleExecuteReducedDataCollection();
   void handleCancelReducedDataCollection();
   void handleBiasRefreshing();
   void handleBiasRefreshed(QString time);
   void handleBiasState(bool biasOn);
   void handleSetFingerTemperature(double temperature);
   void handleMonitorData(MonitorData *md);
   void positionChanged(Motor *motor, const QVariant & value);
   void handleExternalTriggerReceived();
   void handleBufferReady(unsigned char * transferBuffer, unsigned long validFrames);
private slots:
   //void handlePushFilename();
};

#endif // DATAACQUISITION_H
