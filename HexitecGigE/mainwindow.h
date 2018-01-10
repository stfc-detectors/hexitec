/*
 MainWindow class for TwoEasy code

 S D M Jacques 24th February 2011
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QTabWidget>
#include <QWidget>
#include <QAction>
#include <QMenu>
#include <QTextEdit>
#include <QTreeView>
#include <QSettings>

//#include "matlab.h"
#include "applicationoutput.h"
#include "renderarea.h"
#include "plotter.h"
#include "thumbviewer.h"
//#include "scriptingwidget.h"
//#include "motioncontrolform.h"
#include "chargesharing.h"
#include "mainviewer.h"
#include "workspace.h"
#include "dataacquisitionfactory.h"
//#include "processingwindow.h"
//#include "progressform.h"
#include "pixelmanipulationform.h"

class HexitecSoftTrigger;
class HardTrigger;

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow();
   ~MainWindow();

//protected:

private:
   void createMenus();
   QMainWindow *createVisualisation();
   void createStatusBar();
   void createMainViewer();
   void createPixelManipulation();
   void createPlotter();
   void createProgressViewer();
   void createThumbViewer();
   void createWorkSpace();
   void createApplicationOutput();
//   void checkKeithleyConfiguration();
   void closeEvent(QCloseEvent *event);
   bool checkDAQChoice();
   void writeCsv(QString filename, QVector<double> col0, double *col1, int numberOfBins);
   void writeH5(QString fileName);
   QAction *startDAQAct;
   QAction *stopDAQAct;
   QAction *startHVAct;
   QAction *stopHVAct;

//   ProcessingWindow *processingWindow;
   QMainWindow *visualisation;
   QMainWindow *dataAcquisitionWindow;
   ////
//   KeithleyMainWindow *keithleyMainWindow();
   ////
   ThumbViewer *thumbViewer;
   ProgressForm *progressForm;
   Plotter *plotter;
   PixelManipulationForm *pixelManipulationForm;
//   ScriptingWidget *scripting ;
   Workspace *workspace;
   ChargeSharing *chargeSharingInstance;
   bool activeHexitec;
   QMenu *viewMenu;
   QMenu *fileMenu;
   HexitecSoftTrigger *hexitecSoftTrigger;
   HardTrigger *hardTrigger;
//   Keithley *keithley;
//   bool keithleyPresent;
   // Variables needed by hexitech.exe integration
//   bool bHexitechProcessingBusy;
   bool bUpdateVisualisationTab;
   DataAcquisitionFactory *dataAcquisitionFactory;
   QString readDir;
   QString readFilter;
   bool activeDAQ;
   bool hVOn;
   std::ofstream outFile;
   bool saveH5;
   bool saveCsv;

signals:
   void addObject(QObject *object, bool scripting = TRUE, bool gui = TRUE);
   void writeMessage(QString message);
   void writeWarning(QString message);
   void writeError(QString message);
//   // Signal to processingWindow to initialise itself using process.any.ini
//   void initialiseProcessingWindow();
//   // Signal to processingWindow manual processing started
//   void manualProcessingStarted();
//   // Signal to processingWindow manual processing abandoned - re-enable GUI
//   void manualProcessingAbandoned();
//   // Signal to processingWindow->getHxtProcessor to remove unprocessed raw files
//   void removeUnprocessedFiles(bool bRemoveFiles);
   /// HexitechGigE Addition:
//   void hxtProcessingPrepSettings();
   void executeBufferReady(unsigned char * transferBuffer, unsigned long validFrames);
   void executeBufferReady(unsigned char * transferBuffer, unsigned long validFrames, int mode);
   void executeReturnBufferReady(unsigned char * transferBuffer);
   void executeShowImage();
   void startDAQ();
   void stopDAQ();
   void startHV();
   void stopHV();
   void returnHxtBuffer(unsigned short* hxtBuffer);  /// HexitecGigE Addition
//   void updateProgress(double elapsed);
   // The private slots are used only internally to connect to the menus and to other parts of the program
private slots:
   void readFiles();
   void saveFiles();
   void deleteSlice(Slice *slice);
   void deleteActiveSlice();
   void deleteAllSlices();
   void deleteFirstSlice(); // Necessary because cannot do SIGNAL(deleteSlice( DataModel::instance()->sliceAt(0)))...
   void deleteExcessSlices();   // Added 11.04.2014
   void about();
//   void testDevelopment();
//   void getPrinComps();
   void externalChargeShare();
   void initializeSlice(Slice *slice, int sliceNumber = -1);
   void save();
   void handleSpectrumFile(QString fileName);

   // The public slots can also be used from scripting and other parts of the code
public slots:
   void handleStartDAQ();
   void handleStopDAQ();
   void handleStartHV();
   void handleStopHV();
   void readFiles(QStringList files);
   void readBuffer(unsigned short* buffer, QString fileName);   /// HexitecGigE Addition
   void readData(QString fileName);
//   void sendActiveSliceToMatlab();
//   void getActiveSliceFromMatlab();
   // HxtProcessing (from processingWindow) signals background processing thread status
//   void updateHexitechProcessingStatus(bool isBusy);
   // ProcessingWindow signals whether Visualisation tab should be updated
   void updateVisualisationTab(bool bUpdate);
   void handleBufferReady();
//   void handleReturnBufferReady(unsigned char *buffer);
   void handleShowImage();
   void enableMainWindowActions();
   void disableMainWindowActions();
   void disableStopDAQAction();
   void enableHVActions();
   void disableHVActions();
   void handleProcessingComplete(QString fileName);
   void handleSaveH5Changed(bool saveH5);
   void handleSaveCsvChanged(bool saveCsv);
};

#endif
