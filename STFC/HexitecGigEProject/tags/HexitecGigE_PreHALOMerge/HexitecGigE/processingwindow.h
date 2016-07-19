#ifndef PROCESSINGWINDOW_H
#define PROCESSINGWINDOW_H


/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX


#include <QMainWindow>
#include <QPixmap>
#include <QAction>
#include <QMenu>
#include <QWidget>
#include <QTimer>
#include "gigedetector.h"

#include "detectormonitor.h"
#include "monitordata.h"

//DEVELOPMENT:
#include <HxtLogger.h>
#include "HxtRawDataProcessor.h"
#include "hxtprocessing.h"
#include "datamodel.h"
#include <string>
#include <vector>
//Used by doesFilePathExist()
#include <io.h>         // For access().
#include <sys/stat.h>   // For stat().

using namespace std;

class MainWindow;

namespace Ui {
   class ProcessingWindow;
}

class ProcessingWindow : public QWidget
{
   Q_OBJECT
   
public:
   static ProcessingWindow *instance(MainWindow *mw, QWidget *parent = 0);
   static hexitech::HxtProcessing *getHxtProcessor();
   explicit ProcessingWindow(MainWindow *mw, QWidget *parent = 0);
   ~ProcessingWindow();
   QMainWindow *getMainWindow();

   // Provide thread-safe access to HxtProcessing bManualProcessingEnabled
   void toggleManualProcessingNow(bool bManualEnabled);
   // Provide access to HxtProcessing's function with same name
   void setRawFilesToProcess(QStringList rawFilesList);
   // Calculate and update number of [histogram] bins
   void updateNumberOfBins();

private:
   static ProcessingWindow *processingWindowInstance;
   static hexitech::HxtProcessing *HxtProcessor;

   static void initHexitechProcessor();
   static void configHexitechSettings();
   bool areStringsEqual(QString s1, QString s2);
   bool sanityCheckQString(QStringList aList, QString aString);
   bool fileExists(const char* filename);
   int doesFilePathExist(string filePath);
   string stripFilenameFromFullPath(string filePath);
   bool ProcessingWindow::split(const std::string &s, string* sGradient, string* sIntercept);
   string validateFileName(QString* qsKeyName, QString* qsKeyValue);
   void updateCalibrationFileEntry(QString gradientsFile, QString interceptsFile);
   void changeSpectrumOptionsUnitLabels(QString newUnit);
   DetectorMonitor *detectorMonitor;

   Ui::ProcessingWindow *ui;
   QMainWindow *mainWindow;
   IniFile *hexitechIniFile;
   QString hexitechFilename;
   void createActions();
   void createContextMenus();
   void guiAutomaticProcessingChosen();
   void guiManualProcessingChosen();

   QAction *clearAction;
   QMenu *outputContextMenu;
   QStringList modesOptions;
   QStringList frequencyOptions;
   QStringList updateDisplayOptions;
   QStringList yesNoOptions;
   QStringList calibrationOptions;
   QStringList chargeSharingCorrectionOptions;
   QStringList trueFalseOptions;

   bool bPrefixChecked;
   bool bMotorChecked;
   bool bTimeStampChecked;
   bool bCustomChecked;
   bool bUseRawFileChecked;

   processingCondition currentCondition;
   string defaultDecodedFileName;

   /// [Define variables for configuring HxtProcessing instance]
   static unsigned int iDebugLevel;
   static unsigned int iHistoStartVal;
   static unsigned int iHistoEndVal;
   static double dHistoBins;
   static unsigned int iNumberBins; // = (iHistoEndVal - iHistoStartVal)/dHistoBins
   static unsigned int iInterpolationThreshold;
   static double dInducedNoiseThreshold;
   static double dGlobalThreshold;
   static double dDiscWritingInterval;
   static string sThresholdFileName;
   static string sOutputFileNameDecodedFrame;
   static string sGradientsFile;
   static string sInterceptsFile;
   static string sMomentumFile;
   static bool bEnableInCorrector;
   static bool bEnableCabCorrector;
   static bool bEnableCsaspCorrector;
   static bool bEnableCsdCorrector;
   static bool bEnableIdCorrector;
   static bool bEnableIpCorrector;
   static bool bEnableMomCorrector;
   static bool bEnableDbPxlsCorrector;
//   static bool bWriteCsvFiles;
   static bool bEnableVector;
   static bool bEnableDebugFrame;
   /// [End]
   // Debug function:
   void dumpHexitechConfig();
   // Reload Spectrum Options whenever Calibration Checkbox changed
   void reloadSpectrumOptionsDefaultValues();
   // Used to hide information during class initialisation
   bool bSuppressLoadSettingsInfo;

signals:
   void writeMessage(QString message);
   void writeWarning(QString message);
   void removeSlice();
   // Signal to MainWindow whether to (suppress) updating Visualisation tab
   void updateVisualisationSignal(bool updateTab);
   // Signal to HxtProcessing status of bools
   void updatePrefixSignal(bool bPrefixChecked);
   void updateMotorSignal(bool bMotorChecked);
   void updateTimeStampSignal(bool bTimeStampChecked);
   void updateSameAsRawFileSignal(bool bSameAsRawFileChecked);
   // Signal to mainwindow when user may [not] collect data
   void updateMainWindowDataTakingSignal(bool bEnable);
   void updateProcessingConditionSignal(processingCondition newCondition);
   void updateProcessingVeto(bool bVeto);
   void removeUnprocessedFiles(bool bRemoveFiles);
   void customFileSelected(bool bCustom);

protected slots:
   void outputContextMenuEvent(QPoint point);

private slots:
   void handleWriteError(QString message);
   void handleWriteMessage(QString message);

   // Initialise this tab using default settings from processing.ini
   void initialiseProcessingWindow();

   void displayHxtProcessingDatFiles(vector<string> fileNames);
   void displayHxtProcessingBuffers(vector<unsigned short*> bufferNames);    /// HexitecGigE Added
   void displayHxtProcessingHxtFile(string fileName);
   void updateStateLabel(bool isBusy);
   void debugButtonPressed();
   void dataProcessingComboBoxChanged(QString aString);
   void processingFrequencyComboBoxChanged(QString aString);
   void updateDisplayComboBoxChanged(QString aString);
   void filePrefixCheckBoxToggled(bool isChecked);
   void motorPositionCheckBoxToggled(bool isChecked);
   void dataTimeStampCheckBoxToggled(bool isChecked);
   void customCheckBoxToggled(bool isChecked);
   void sameAsRawFileCheckBoxToggled(bool isChecked);
   void calibrationComboBoxChanged(QString aString);
   void nextFrameCorrectionComboBoxChanged(QString aString);
   void chargeSharingCorrectionComboBoxChanged(QString aString);
   void hxtFileLineEditChanged(QString newPrefixString);
   void hxtFileSaveButtonPressed();
   void momentumBrowseButtonPressed();
   void gradientsBrowseButtonPressed();
   void interceptsBrowseButtonPressed();
   void globalThresholdBrowseButtonPressed();
   void momentumPathLineEditChanged(const QString &momentumFile);
   void gradientsPathLineEditChanged(const QString &gradientsFile);
   void interceptsPathLineEditChanged(const QString &interceptsFile);
   void globalThresholdPathLineEditChanged(const QString &globalThresholdFile);
   void startBinLineEditChanged(QString aString);
   void endBinLineEditChanged(QString aString);
   void binWidthLineEditChanged(QString aString);
   void thresholdLineEditChanged(QString aString);
   void loadSettingsButtonPressed();
   void saveSettingsButtonPressed();
   // Allow processNowButton disable/re-enable GUI while
   //   HxtProcessor work in the background
   void guiProcessNowStarted();
   void guiProcessNowFinished();
   // Toggle GUI components according to unprocessed files
   //   available in the queue of HxtProcessor
   void guiUnprocessedFilesInQueue(bool bFilesPresent);
   // Setup slot to communicate to HxtProcessing to discard all unprocessed raw files
   void clearUnprocessedFiles();
   /// Temporary slot to demonstrate changed HXT file name working
   void handleHxtProcessedFileNameChanged(string hxtFileNameCompleted);
};

#endif // PROCESSINGWINDOW_H
