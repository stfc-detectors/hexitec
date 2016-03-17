#ifndef GIGEDETECTOR_H
#define GIGEDETECTOR_H

#include <QObject>
#include <QPixmap>
#include <fstream>
#include <QDateTime>
#include <string>

#include "detectorexception.h"
#include "inifile.h"
#include "windowsevent.h"
#include "GigE.h"

#define HEXITEC_BUFFER_READY TEXT("Hexitec_BufferReady")
#define HEXITEC_RETURN_BUFFER_READY TEXT("Hexitec_ReturnBufferReady")
#define HEXITEC_STOP_DAQ TEXT("Hexitec_StopDAQ")
#define HEXITEC_SHOW_IMAGE TEXT("Hexitec_ShowImage")

class GigEDetector : public QObject
{
   Q_OBJECT

public:
/* If change add a Mode be sure to add to modes variable. */
   enum Mode {CONTINUOUS, GIGE_DEFAULT, INVALID_MODE};
/**********************************************************/
   enum DetectorState {IDLE, READY, INITIALISING, INITIALISED, WAITING_DARK, OFFSETS, OFFSETS_PREP, COLLECTING_PREP, COLLECTING, WAITING_TRIGGER, TRIGGERING_STOPPED};
   enum DetectorCommand {CONNECT, CONFIGURE, RECONFIGURE, INITIALISE, COLLECT, COLLECT_OFFSETS, TRIGGER, ABORT, CLOSE, KILL, STATE, STOP_TRIGGER};
   GigEDetector(QString aspectFilename, const QObject *parent = 0);
   ~GigEDetector();

   Q_INVOKABLE int initialiseConnection();
   Q_INVOKABLE int terminateConnection();
   Q_INVOKABLE int getDetectorValues(double *rh, double *th, double *tasic, double *tdac, double *hv, double *t);
   Q_INVOKABLE void getImages(int count, int ndaq);
   Q_INVOKABLE void enableDarks();
   Q_INVOKABLE void disableDarks();

   unsigned int getXResolution();
   unsigned int getYResolution();
   void setXResolution(unsigned int xResolution);
   void setYResolution(unsigned int yResolution);
   void setMode(Mode mode);
   void setBufferReadyEvent();
   QString getDirectory();
   QString getPrefix();
   bool getTimestampOn();
   void setTimestampOn(bool timestampOn);
   void setDirectory(QString directory);
   void setPrefix(QString prefix);
   void setDataAcquisitionDuration(double imageAcquisitionDuration);
   void acquireImages();
   int getLoggingInterval();
   void beginMonitoring();

   WindowsEvent *getBufferReadyEvent();
   WindowsEvent *getReturnBufferReadyEvent();
   WindowsEvent *getShowImageEvent();
   static PUCHAR getBufferReady();
   static ULONG getValidFrames();
   void abort(bool restart);

signals:
   void notifyState(GigEDetector::DetectorState state);
   void writeError(QString message);
   void writeMessage(QString message);
   void executeCommand(GigEDetector::DetectorCommand, int, int);
   void executeGetImages();
   void executeReturnBufferReady(unsigned char * transferBuffer);
   void executeBufferReady(unsigned char * transferBuffer, unsigned long validFrames);
   void notifyStop();
   void imageAcquired(QPixmap data);
   void imageStarted(char *path, int frameSize);
   void imageComplete(unsigned long long framesAcquired);
   void executeAcquireImages();
   void prepareForOffsets();
   void prepareForDataCollection();
   void enableMonitoring();

public slots:
   void handleShowImage();
   void handleExecuteCommand(GigEDetector::DetectorCommand command, int ival1, int ival2 = 1);
   void handleExecuteGetImages();
   void handleStop();
   void handleReducedDataCollection();
   void handleExecuteOffsets();
   void handleBufferReady();
   void handleReturnBufferReady();
   void handleReturnBufferReady(unsigned char *returnBuffer, unsigned long validFrames);
   void handleSetTargetTemperature(double targetTemperature);
   void handleSetHV(double voltage);
   void handleAppendTimestamp(bool appendTimestamp);
   void handleSaveRawChanged(bool saveRaw);

private:
   QThread *gigEDetectorThread;
   DetectorState state;
   unsigned int xResolution;
   unsigned int yResolution;
   int frameSize;
   std::ofstream outFile;
   bool appendTimestamp;
   bool saveRaw;
   QString errorMessage;

   WindowsEvent *bufferReadyEvent;
   WindowsEvent *returnBufferReadyEvent;
   WindowsEvent *showImageEvent;
   const QObject *parent;
   Mode mode;
   bool offsetsOn;
   bool triggered;
   double hvSetPoint;
   double targetTemperature;
   double vCal;
   double uMid;
   double detCtrl;

   bool timestampOn;
   int count;
   QString directory;
   QString prefix;
   char pathString[1024];
   char processingFilename[1024];
   // Data acquisition time in mSecs
   double dataAcquisitionDuration;
   unsigned char xRes, xResAcquiredImage;
   unsigned char yRes, yResAcquiredImage;
   ULONG collectDcTime;
   double frameTime;
   int imgCntAverage;
   ULONG timeout;
   short *imageDest;
   int *summedImageDest;
   unsigned char *charImageDest;
   QPixmap imagePixmap;
   unsigned long framesPerBuffer;
   int loggingInterval;
   QString aspectFilename;
   IniFile *iniFile;
   HexitecSensorConfig sensorConfig;
   HexitecSetupRegister rowSetupRegister;
   HexitecSetupRegister columnSetupRegister;
   HANDLE detectorHandle;

   void connectUp(const QObject *parent);
   LONG readIniFile(QString aspectFilename);
   HexitecSetupRegister initSetupRegister(QString type);
   void configCharacters2Bytes(std::string configCharacters, unsigned char *result);
   int setImageFormat(unsigned long xResolution, unsigned long yResolution);
   void updateState(DetectorState state);
   void imageDestToPixmap();
   unsigned char *getImage(int imageNumber);
   void run();
   void showError(const LPSTR context, long asError);
   LONG collectOffsets();
   void setGetImageParams();
};

#endif // GIGEDETECTOR_H
