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

#include "matlab.h"
#include "applicationoutput.h"
#include "renderarea.h"
#include "plotter.h"
#include "thumbviewer.h"
#include "scriptingwidget.h"
#include "motioncontrolform.h"
#include "chargesharing.h"
#include "mainviewer.h"
#include "workspace.h"
///#include "keithleymainwindow.h"
#include "dataacquisitionfactory.h"
#include "processingwindow.h"

class HexitecSoftTrigger;
class HardTrigger;
class Keithley;
class KeithleyMainWindow;

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
   void createPlotter();
   void createThumbViewer();
   void createWorkSpace();
   void createApplicationOutput();
   void checkKeithleyConfiguration();
   void closeEvent(QCloseEvent *event);
   bool checkDAQChoice();
   QAction *startDAQAct;
   QAction *stopDAQAct;

   ProcessingWindow *processingWindow;
   QMainWindow *visualisation;
   QMainWindow *dataAcquisitionWindow;
   ////
   KeithleyMainWindow *keithleyMainWindow();
   ////
   ThumbViewer *thumbViewer;
   Plotter *plotter;
   ScriptingWidget *scripting ;
   Workspace *workspace;
   ChargeSharing *chargeSharingInstance;
   bool activeHexitec;
   QMenu *viewMenu;
   QMenu *fileMenu;
   HexitecSoftTrigger *hexitecSoftTrigger;
   HardTrigger *hardTrigger;
   Keithley *keithley;
   bool keithleyPresent;
   // Variables needed by hexitech.exe integration
   bool bHexitechProcessingBusy;
   bool bUpdateVisualisationTab;
   DataAcquisitionFactory *dataAcquisitionFactory;
   QString readDir;
   QString readFilter;
   bool activeDAQ;

signals:
   void addObject(QObject *object, bool scripting = TRUE, bool gui = TRUE);
   void writeMessage(QString message);
   void writeWarning(QString message);
   void writeError(QString message);
   // Signal to processingWindow to initialise itself using process.any.ini
   void initialiseProcessingWindow();
   // Signal to processingWindow manual processing started
   void manualProcessingStarted();
   // Signal to processingWindow manual processing abandoned - re-enable GUI
   void manualProcessingAbandoned();
   // Signal to processingWindow->getHxtProcessor to remove unprocessed raw files
   void removeUnprocessedFiles(bool bRemoveFiles);
   void executeBufferReady(unsigned char * transferBuffer, unsigned long validFrames);
   void executeReturnBufferReady(unsigned char * transferBuffer);
   void executeShowImage();
   void startDAQ();
   void stopDAQ();
   void returnHxtBuffer(unsigned short* hxtBuffer);  /// HexitecGigE Addition

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
   void testDevelopment();
   void getPrinComps();
   void externalChargeShare();
   void initializeSlice(Slice *slice);
   void save();
   // Called by processNowButton in processingWindow
   void processNow();
   void handleSpectrumFile(QString fileName);

   // The public slots can also be used from scripting and other parts of the code
public slots:
   void handleStartDAQ();
   void handleStopDAQ();
   void readFiles(QStringList files);
   void readBuffer(unsigned short* buffer);   /// HexitecGigE Addition
   void readData(QString fileName);
   void sendActiveSliceToMatlab();
   void getActiveSliceFromMatlab();
   // HxtProcessing (from processingWindow) signals background processing thread status
   void updateHexitechProcessingStatus(bool isBusy);
   // ProcessingWindow signals whether Visualisation tab should be updated
   void updateVisualisationTab(bool bUpdate);
   void handleBufferReady();
//   void handleReturnBufferReady(unsigned char *buffer);
   void handleShowImage();
   void enableMainWindowActions();
   void disableMainWindowActions();
};

#endif