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

#include "applicationoutput.h"
#include "renderarea.h"
#include "plotter.h"
#include "thumbviewer.h"
#include "chargesharing.h"
#include "mainviewer.h"
#include "workspace.h"
#include "dataacquisitionfactory.h"
#include "pixelmanipulationform.h"
///
#include <QMutex>

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
   void closeEvent(QCloseEvent *event);
   bool checkDAQChoice();
   void writeCsv(QString filename, QVector<double> col0, double *col1, int numberOfBins);
//   void writeH5(QString fileName);
   QAction *startDAQAct;
   QAction *stopDAQAct;
   QAction *startHVAct;
   QAction *stopHVAct;

   QMainWindow *visualisation;
   QMainWindow *dataAcquisitionWindow;
   ThumbViewer *thumbViewer;
   ProgressForm *progressForm;
   Plotter *plotter;
   PixelManipulationForm *pixelManipulationForm;
   Workspace *workspace;
   ChargeSharing *chargeSharingInstance;
   bool activeHexitec;
   QMenu *viewMenu;
   QMenu *fileMenu;
   HexitecSoftTrigger *hexitecSoftTrigger;
   HardTrigger *hardTrigger;
   DataAcquisitionFactory *dataAcquisitionFactory;
   QString readDir;
   QString readFilter;
   bool activeDAQ;
   bool hVOn;
   std::ofstream outFile;
//   bool saveH5;
   ///
   bool bMainWindowBusy;
   QMutex busyMutex;

signals:
   void addObject(QObject *object, bool scripting = TRUE, bool gui = TRUE);
   void writeMessage(QString message);
   void writeWarning(QString message);
   void writeError(QString message);
   void executeBufferReady(unsigned char * transferBuffer, unsigned long validFrames);
   void executeBufferReady(unsigned char * transferBuffer, unsigned long validFrames, int mode);
   void executeReturnBufferReady(unsigned char * transferBuffer);
   void executeShowImage();
   void startDAQ();
   void stopDAQ();
   void startHV();
   void stopHV();
   void mainWindowBusy(bool bBusy);
   // The private slots are used only internally to connect to the menus and to other parts of the program
private slots:
   void readFiles();
//   void saveFiles();
   void deleteSlice(Slice *slice);
   void deleteActiveSlice();
   void deleteAllSlices();
   void deleteFirstSlice(); // Necessary because cannot do SIGNAL(deleteSlice( DataModel::instance()->sliceAt(0)))...
   void deleteExcessSlices();   // Added 11.04.2014
   void about();
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
   void handleBufferReady();
   void handleShowImage();
   void enableMainWindowActions();
   void disableMainWindowActions();
   void disableStopDAQAction();
   void enableHVActions();
   void disableHVActions();
   /// Allow ImageProcessor (via ProcessingBufferGenerator) to signal occupancy corrections
   void handleOccupancyCorrections(int occupancyThreshold, int corrections);
};

#endif
