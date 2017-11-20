#ifndef DETECTORCONTROLFORM_H
#define DETECTORCONTROLFORM_H

#include <QMainWindow>
#include <QWidget>
#include "hv.h"
#include "monitordata.h"
#include "dpwarningdialog.h"
#include "gigedetector.h"
#include "dataacquisitionstatus.h"

namespace Ui {
class DetectorControlForm;
}

class DetectorControlForm : public QWidget
{
   Q_OBJECT

public:
   explicit DetectorControlForm(QWidget *parent = 0);
   ~DetectorControlForm();
   void initialiseFailed();
   QMainWindow *getMainWindow();
   void setHvName(QString hvName);
   QString hvName;

private:
   Ui::DetectorControlForm *ui;
   QMainWindow *mainWindow;
   DPWarningDialog *dpWarningDialog;
   bool waitingForModeChange;
   bool hvReservedByScripting;
   bool tAboveTdp;
   bool firstMonitor;
   bool triggeringAvailable;
   double targetTemperature;
   double targetTemperatureMin;
   double targetTemperatureMax;
   double triggerTimeout;
   void readIniFile();
   void connectSignals();
   void terminateDetector();
   void guiIdle();
   void guiReady();
   void guiInitialising();
   void guiWaitingDarks();
   void guiOffsets();
   void guiOffsetsPrep();
   void guiCollectingPrep();
   void guiCollecting();
   void guiDetectorBusy();
   void guiBiasRefreshing();

public slots:
    void handleMonitorData(MonitorData *md);
    void handleSetFingerTemperature();
    void handleTemperatureBelowDP();
    void handleTemperatureAboveDP();
    void setPixmap(QPixmap pixmap);
    void handleBiasRefreshed(QString time, bool restartMonitoring);
    void handleCollectingChanged(bool collectingOn);
    void handleDataAcquisitionStatusChanged(DataAcquisitionStatus status);
    void handleBiasVoltageChanged(bool biasOn);
    void handleTriggeringSelectionChanged(int triggering);
    void handleTtlInputSelectionChanged(int ttlInput);
    void handleSetTriggerTimeout();
    void handleHVOn();
    void handleHVOff();
    void handleScriptReserve(QString name);
    void handleScriptRelease(QString name);
    void handleTriggeringAvailable(bool triggeringAvailable);

private slots:
   void biasVoltageClicked(bool biasVoltageOn);
   void handleCollectImagesPressed();
   void initialiseDetectorPressed();
   void terminateDetectorPressed();
   void abortDAQ();
   void handleFixedImageCountChanged(int fixedImageCount);

signals:
   void executeCommand(HV::VoltageSourceCommand);
   void executeCommand(GigEDetector::DetectorCommand, int, int);
   void collectImagesPressed();
   void initialiseDetector();
   void abortDAQPressed();
   void fixedImageCountChanged(int);
   void setFingerTemperature(double);
   void biasVoltageChanged(bool);
   void disableMonitoring();
   void disableBiasRefresh();
   void triggeringSelectionChanged(int triggering);
   void ttlInputSelectionChanged(int ttlInput);
   void setTriggerTimeout(double);
   void writeError(QString message);
   void writeMessage(QString message);
   void enableHVActions();
   void disableHVActions();
};

#endif // DETECTORCONTROLFORM_H
