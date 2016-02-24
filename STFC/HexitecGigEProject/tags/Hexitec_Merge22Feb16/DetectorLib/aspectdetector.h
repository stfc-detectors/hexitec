#ifndef ASPECTDETECTOR_H
#define ASPECTDETECTOR_H

#include <stddef.h>
#include <QStringList>
//#include <QBasicTimer>
#include <QTimerEvent>
//#include <QTime>
#include <QPixmap>
#include <QThread>
#include <QString>

//#include "detector.h"
#include "inifile.h"
#include "windowsevent.h"
#include "Hexitec.h"
//#include "offsetsdialog.h"

#define MINIMUM_FIRMWARE_VERSION 8
#define SENSOR_20x20 0
#define SENSOR_80x80 32
#define SENSOR_80x80_2x2 16
#define FPGA_OFFSET 0x30
#define CAMLINK_OFFSET 0x40
#define DAC_OFFSET 0x50
#define ADC1_OFFSET 0x60
#define ADC2_OFFSET 0x61
#define ANALOGUE_MODE "00"
#define REDUCED_DATA_MODE "7A"
#define ASPECT_LEN_ERR_MSG 512
#define ASPECT_LEN_SERIAL_MSG 512
#define SYNCHRONOUS_RAW 0
#define ASYNCHRONOUS_RAW 1
#define SYNCHRONOUS_REDUCED 2
#define ASYNCHRONOUS_REDUCED 3
#define HEXITEC_TRIGGERED TEXT("Hexitec_Triggered")
#define HEXITEC_TRIGGERED_REDUCED_DATA TEXT("Hexitec_TriggeredReducedData")
#define HEXITEC_START_TRIGGER TEXT("Hexitec_StartTrigger")
#define HEXITEC_SHOW_IMAGE TEXT("Hexitec_ShowImage")
#define HEXITEC_STOP TEXT("Hexitec_Stop")
#define HEXITEC_STOP_GRAB TEXT("Hexitec_StopGrab")
#define HEXITEC_SAVE_BUFFER TEXT("Hexitec_SaveBuffer")

class AspectDetector : public QObject
{
   Q_OBJECT
enum sensorMode {ANALOGUE, REDUCED_DATA};
enum frameMode {ANALOGUE_1_FRAME_DATA, ANALOGUE_3_FRAME_DATA, REDUCED_FRAME_DATA};
public:
/* If change add a Mode be sure to add to modes variable. */
enum Mode {CONTINUOUS, SOFT_TRIGGER, EXTERNAL_TRIGGER, FIXED, INVALID_MODE};
/**********************************************************/
enum DetectorState {IDLE, READY, INITIALISING, INITIALISED, WAITING_DARK, OFFSETS, OFFSETS_PREP, COLLECTING_PREP, COLLECTING, WAITING_TRIGGER, TRIGGERING_STOPPED};
enum DetectorCommand {CONNECT, CONFIGURE, RECONFIGURE, INITIALISE, COLLECT, COLLECT_OFFSETS, TRIGGER, ABORT, CLOSE, KILL, STATE, STOP_TRIGGER};
   AspectDetector(QString aspectFilename, QString detectorFilename, Mode mode = CONTINUOUS, int quadrant = 1);
   ~AspectDetector();
   Q_INVOKABLE int current(double *temperature);
   Q_INVOKABLE int getImages(int count, int ndaq);
   Q_INVOKABLE Mode getMode();
   Q_INVOKABLE int configure(int mode);
   Q_INVOKABLE int initialiseConnection();
   Q_INVOKABLE int terminateConnection();
   Q_INVOKABLE void enableDarks();
   Q_INVOKABLE void disableDarks();
   Q_INVOKABLE int abort(bool restart);
   QSize getSize();
   Q_INVOKABLE void setMode(Mode mode);
   Q_INVOKABLE void setQuadrant(int quadrant);
   Q_INVOKABLE QString getState(AspectDetector::DetectorState state);
   QStringList getModes();
   QStringList getReducedDataModes();
   QString getDirectory();
   QString getPrefix();
   bool getTimestampOn();
   void setTimestampOn(bool timestampOn);
   void setDirectory(QString directory);
   void setPrefix(QString prefix);
   void timerEvent(QTimerEvent *event);
   void setDataAcquisitionDuration(double imageAcquisitionDuration);
   QString getAspectIniFilename();
   QString getDetectorIniFilename();
   bool isReducedDataMode(Mode mode);
   int getTimeError();
   void registerCallback(p_newDataFileFunction fileCallback);
   void unRegisterCallback();
   //LONG fileCallback(CONST LPSTR path);
signals:
   void notifyState(AspectDetector::DetectorState);
   void notifyMode(AspectDetector::Mode);
   void writeError(QString message);
   void writeMessage(QString message);
   void imageAcquired(QPixmap data);
   void image1Acquired(QPixmap data);
   void image2Acquired(QPixmap data);
   void image3Acquired(QPixmap data);
   void executeCommand(AspectDetector::DetectorCommand, int, int);
   void executeGetImages();
   void prepareForOffsets();
   void prepareForDataCollection();
   void externalTriggerReceived();
public slots:
   void handleShowImage();
   void handleTriggered();
   void handleTriggeredReducedData();
   void handleStop();
   void handleExecuteCommand(AspectDetector::DetectorCommand command, int ival1, int ival2 = 1);
   void handleExecuteGetImages();
   void handleReducedDataCollection();
   void handleExecuteOffsets();
   void offsetsDialogAccepted();
private:
   QThread *aspectDetectorThread;
   DetectorState state;
   QStringList rowNameEnds;
   QStringList columnNameEnds;
   AHANDLE detectorHandle;
   bool connected;
   bool firstRunAfterInit;
   UCHAR idx;
   ULONG timeout;
   ULONG reducedDataTimeout;
   IniFile *iniFile;
   IniFile *detectorIniFile;
   Mode mode;
   int quadrant;
   bool timestampOn;
   QStringList modes;
   QStringList reducedDataModes;
   int sensorMode;
   int frameMode;
   int xRes, xResAcquiredImage;
   int yRes, yResAcquiredImage;
   ImageInfoPtr imageInfoPtr;
   short *imageDest;
   int *summedImageDest;
   unsigned char *charImageDest;
   int imageSize;
   bool offsetsOn;
   bool triggered;
   //short imageDest[4740];
   QPixmap imagePixmap;
   WindowsEvent *stopEvent;
   WindowsEvent *stopGrabEvent;
   WindowsEvent *showImageEvent;
   WindowsEvent *startTriggerEvent;
   WindowsEvent *triggeredEvent;
   WindowsEvent *triggeredReducedDataEvent;
   WindowsEvent *saveBufferEvent;
   UCHAR streaming;
   int count;
   QString directory;
   QString prefix;
   char pathString[256];
   // Data acquisition time in mSecs
   double dataAcquisitionDuration;
   double frameTime;
   int imgCntAverage;
   int getImagesTimerEventId;
   qint64 startTime;
   QString aspectFilename;
   QString detectorFilename;
   int timeError;

   LONG readIniFiles(QString aspectFilename, QString detectorFilename);
   void connectUp();
   LONG serialWrite(CONST PUCHAR buffer, PULONG bufferLen);
   LONG serialRead(PUCHAR buffer, PULONG bufferLen);
   LONG readRegister(CHAR offset, int *value);
   LONG writeFPGARegister(CHAR *registerNumber, CHAR *registerValue);
   LONG writeCamLinkRegister();
   LONG writeDACRegister(QString registerValues);
   LONG writeADC1Register(CHAR *registerNumber, CHAR *registerValue);
   LONG writeADC2Register(CHAR *registerNumber, CHAR *registerValue);
   LONG initialiseModules();
   LONG exitModules();
   LONG initFPGA();
   LONG exitFPGA();
   LONG initCameraLink();
   LONG exitCameraLink();
   LONG initDAC();
   LONG exitDAC();
   LONG initADC1();
   LONG exitADC1();
   LONG initADC2();
   LONG exitADC2();
   LONG init_80x80_2x2_Sensor();
   LONG terminate_80x80_2x2_Sensor();
   LONG initFPGARegisters();
   LONG writeDACConfiguration();
   LONG calcDACChannelValue(float voltage, QString *value, int channelNumber);
   LONG setRegisterBlock(int startReg, QString block);
   LONG initRegisterBlock(int startReg, QString nameStart);
   LONG firmwareVersion(int *version);
   LONG firmwareProjectId();
   LONG firmwareCustomerId();
   LONG getRows();
   LONG getColumns();
   LONG getFPGARegister(CHAR *regNo, int *value);
   LONG getFPGARegister(CHAR *regNo, QString *value);
   LONG configureFrameGrabber();
   LONG collectOffsets();
   LONG restartReducedDataMode();
   LONG startReducedDataStateMachine();
   double calculateFrameTime();
   void analyseErrorStatus(long status, QString location);
   void analyseErrorStatus(long asErr, long niErr, long sysErr);
   void getLastErrorString(long asErr, long niErr, long sysErr);
   LONG breakConnection();
   void emitSensorType(int sensorType);
   QString twoCharHexNumber(int number);
   unsigned int reverseBits(unsigned int num);
   void imageDestToPixmap();
   unsigned char *getImage(int imageNumber);
   void getOffsets();
   void writeImages();
   void zeroSummedImageDest();
   int waitForEvent(HANDLE handle, QString name);
   int waitForEvent(WindowsEvent *event);
   int asynchronousGetResult();
   void writeRegister00();
   void updateState(DetectorState state);
   int triggerCollection();
   void setGetImageParams();
   void configure(int mode, int quadrant);
};

#endif // ASPECTDETECTOR_H
