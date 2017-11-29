#ifndef DATAACQUISITIONFORM_H
#define DATAACQUISITIONFORM_H

#include <QMainWindow>
#include <QWidget>
#include <QTime>

#include "monitordata.h"
#include "gigedetector.h"
#include "detectorfilename.h"
#include "dataacquisitiondefinition.h"
#include "offsetsdialog.h"
#include "datacollectiondialog.h"
#include "dataacquisitionstatus.h"

namespace Ui {
class DataAcquisitionForm;
}
const int seconds_in_day = 86400;
const int hours_in_day = 24;
const int mins_in_hour = 60;
const int secs_in_min = 60;
const int millisecs_in_sec = 1000;

class DataAcquisitionForm : public QWidget
{
   Q_OBJECT

public:
   explicit DataAcquisitionForm(QWidget *parent = 0);
   ~DataAcquisitionForm();
   QMainWindow *getMainWindow();
   void setDaqName(QString daqName);
   void setDaqModelName(QString daqModelName);
private:
   Ui::DataAcquisitionForm *ui;
   QMainWindow *mainWindow;
   QString invalidItemText;
   DetectorFilename *dataFilename;
   DetectorFilename *logFilename;
   DataAcquisitionDefinition dataAcquisitionDefinition;
   OffsetsDialog *offsetsDialog;
   DataCollectionDialog *dataCollectionDialog;
   double daqDuration;
   bool loggingEnabled;
   bool operatedForScripting;
   QString daqName;
   QString daqModelName;
   void connectSignals();
   void guiIdle();
   void guiReady();
   void guiInitialising();
   void guiWaitingDarks();
   void guiWaitingTriggers();
   void guiOffsets();
   void guiOffsetsPrep();
   void guiCollectingPrep();
   void guiCollecting();
   void guiDetectorBusy();
   void guiMode();
   void guiBiasRefreshing();
   void enableRepeats();
   void disableRepeats();
   void disableGui();
   void enableLogfileParameters(bool enabled);
   QString prepend0(QString input, int number);

public slots:
   void handleMonitorData(MonitorData *md);
//   void handleModeChanged(GigEDetector::Mode mode);
   void handleDataChanged(DataAcquisitionDefinition dataAcquisitionDefinition);
//   void handleDataChanged(QString mode);
   void handleBiasRefreshed(QString time, bool restartMonitoring);
   void handleCollectingChanged(bool collectingOn);
   void handleDataAcquisitionStatusChanged(DataAcquisitionStatus status);
   void handleTriggeringSelectionChanged(int triggering);
   void handleTtlInputSelectionChanged(int ttlInput);
private slots:
//   void modeChanged(int mode);
   void handleCollectImagesPressed();
   void handleAbortDAQPressed();
   void handleDataFilename();
   void handleLogFilename();
   void setDataDirectory();
   void setLogDirectory();
   void handleSaveRawChanged(bool saveRaw);
   void handleDataAcquisitionDefinition();
   void prepareForOffsets();
   void prepareForDataCollection();
   void offsetsDialogAccepted();
   void offsetsDialogRejected();
   void dataCollectionDialogAccepted();
   void dataCollectionDialogRejected();
   void handleLoggingEnabled(int loggingEnabled);
   void handleDaqDurationChanged(double daqDuration);
   void handleControlledByGui();
   void handleScriptRelease(QString name);

signals:
   void executeCommand(GigEDetector::DetectorCommand command, int mode);
   void dataFilenameChanged(DetectorFilename dataFilename);
   void logFilenameChanged(DetectorFilename logFilename);
   void dataAcquisitionDefinitionChanged(DataAcquisitionDefinition dataAcquisitionDefinition);
   void collectImagesPressed();
   void getCurrentPressed();
   void registerCallbackPressed();
   void executeOffsets();
   void cancelOffsets();
   void executeReducedDataCollection();
   void cancelReducedDataCollection();
   void abortDAQPressed();
   void createLogFile(DetectorFilename *logFilename);
   void enableMainWindowActions();
   void disableMainWindowActions();
   void disableStopDAQAction();
   void saveRawChanged(bool saveRaw);

   void newDataAcquisitionState(QString state);
   void newDataAcquisitionProgressBarValue(int progressBarValue);
   void newDataAcquisitionImageProgressValue(int imageProgressValue);
};

#endif // DATAACQUISITIONFORM_H
