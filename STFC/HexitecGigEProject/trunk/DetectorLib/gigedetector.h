#ifndef GIGEDETECTOR_H
#define GIGEDETECTOR_H

#include <QObject>
#include <QPixmap>

#include "windowsevent.h"

#define HEXITEC_BUFFER_READY TEXT("Hexitec_BufferReady")
#define HEXITEC_STOP_DAQ TEXT("Hexitec_StopDAQ")
#define HEXITEC_SHOW_IMAGE TEXT("Hexitec_ShowImage")

class GigEDetector : public QObject
{
   Q_OBJECT

public:
/* If change add a Mode be sure to add to modes variable. */
enum Mode {CONTINUOUS, SOFT_TRIGGER, EXTERNAL_TRIGGER, FIXED, GIGE_DEFAULT, INVALID_MODE};
/**********************************************************/
enum DetectorState {IDLE, READY, INITIALISING, INITIALISED, WAITING_DARK, OFFSETS, OFFSETS_PREP, COLLECTING_PREP, COLLECTING, WAITING_TRIGGER, TRIGGERING_STOPPED};
enum DetectorCommand {CONNECT, CONFIGURE, RECONFIGURE, INITIALISE, COLLECT, COLLECT_OFFSETS, TRIGGER, ABORT, CLOSE, KILL, STATE, STOP_TRIGGER};
   GigEDetector(const QObject *parent = 0);
   ~GigEDetector();
   int initialiseConnection();
   int terminateConnection();
   int configure(unsigned long xResolution, unsigned long yResolution);
   unsigned int getXResolution();
   unsigned int getYResolution();
   void setXResolution(unsigned int xResolution);
   void setYResolution(unsigned int yResolution);
   void getImages(int count, int ndaq);
   Mode getMode();
   void setMode(Mode mode);
   void enableDarks();
   void disableDarks();
   void setBufferReadyEvent();
   QString getDirectory();
   QString getPrefix();
   bool getTimestampOn();
   void setTimestampOn(bool timestampOn);
   void setDirectory(QString directory);
   void setPrefix(QString prefix);
//   void timerEvent(QTimerEvent *event);
   void setDataAcquisitionDuration(double imageAcquisitionDuration);
   void acquireImages();

   WindowsEvent *getBufferReadyEvent();
   //WindowsEvent *getStopDAQEvent();
   WindowsEvent *getShowImageEvent();
   static PUCHAR getBufferReady();
   void abort(bool restart);
   static void handleReturnBufferReady(unsigned char *transferBuffer);
signals:
   void notifyState(GigEDetector::DetectorState state);
   void notifyMode(GigEDetector::Mode mode);
   void writeError(QString message);
   void writeMessage(QString message);
   void executeCommand(GigEDetector::DetectorCommand, int, int);
   void executeGetImages(int count, int ndaq);
   void executeReturnBuffer(void * detectorHandle, unsigned char * transferBuffer);
   void executeBufferReady(unsigned char * transferBuffer);
   void notifyStop();
   void imageAcquired(QPixmap data);
   void executeAcquireImages();
 public slots:
   void handleShowImage();
   void handleExecuteCommand(GigEDetector::DetectorCommand command, int ival1, int ival2 = 1);
   void handleExecuteGetImages(int count, int ndaq);   
   void handleStop();
   void handleBufferReady();
private:
   QThread *gigEDetectorThread;
   DetectorState state;
   unsigned int xResolution;
   unsigned int yResolution;
   WindowsEvent *bufferReadyEvent;
   //WindowsEvent *stopDAQEvent;
   WindowsEvent *showImageEvent;
   const QObject *parent;
   Mode mode;
   bool offsetsOn;
   bool triggered;

   bool timestampOn;
   int count;
   QString directory;
   QString prefix;
   //char pathString[256];
   // Data acquisition time in mSecs
   double dataAcquisitionDuration;
   int xRes, xResAcquiredImage;
   int yRes, yResAcquiredImage;
//   ImageInfoPtr imageInfoPtr;
   short *imageDest;
   int *summedImageDest;
   unsigned char *charImageDest;
   QPixmap imagePixmap;


   int setImageFormat(unsigned long xResolution, unsigned long yResolution);  
   void connectUp(const QObject *parent);
   void updateState(DetectorState state);
   void imageDestToPixmap();
   unsigned char *getImage(int imageNumber);
   void run();
};

#endif // GIGEDETECTOR_H
