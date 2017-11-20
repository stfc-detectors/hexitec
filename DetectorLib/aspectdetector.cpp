#include <QDebug>
/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX
//
#include <Windows.h>
#include <QtTest/QTest>
#include <QImage>
#include <QWinEventNotifier>
#include "aspectdetector.h"
#include <QThread>
#include <QDebug>

/********************************
 * Constuctors and destructors. *
 ********************************/

AspectDetector::AspectDetector(QString aspectFilename, QString detectorFilename, Mode mode, int quadrant)
{
   setMode(mode);
   this->quadrant = quadrant;
   this->aspectFilename = aspectFilename;
   this->detectorFilename = detectorFilename;

   timeout = 1000;
   modes << "Fixed" << "Continuous" << "Soft Trigger" << "External Trigger";
   reducedDataModes << "Continuous" << "Soft Trigger" << "External Trigger";
   connected = false;
   offsetsOn = true;
   rowNameEnds << "1stBlock" << "2ndBlock" << "3rdBlock" << "4thBlock";
   columnNameEnds << "1stChannel" << "2ndChannel" << "3rdChannel" << "4thChannel";
   firstRunAfterInit = false;
   imageSize = 0;
   imageDest = NULL;
   summedImageDest = NULL;
   charImageDest = NULL;
   imageInfoPtr = NULL;
   timestampOn = true;

   aspectDetectorThread = new QThread();
   aspectDetectorThread->start();
   moveToThread(aspectDetectorThread);

   readIniFiles(aspectFilename, detectorFilename);

   qRegisterMetaType<AspectDetector::DetectorCommand>("AspectDetector::DetectorCommand");
   qRegisterMetaType<AspectDetector::DetectorState>("AspectDetector::DetectorState");
   qRegisterMetaType<AspectDetector::DetectorState>("AspectDetector::Mode");
   qRegisterMetaType<HANDLE>("HANDLE");

   connect(this, SIGNAL(executeCommand(AspectDetector::DetectorCommand, int, int)), this, SLOT(handleExecuteCommand(AspectDetector::DetectorCommand, int, int)));
   frameTime = calculateFrameTime();
   setMode(INVALID_MODE);
   emit executeCommand(CONNECT, 0, 0);
   emit executeCommand(CONFIGURE, mode, quadrant);

   //   qDebug() << " AspectDetector created in thread id " << QThread::currentThreadId();
}

AspectDetector::~AspectDetector()
{
   if (connected)
   {
      terminateConnection();
      connected = false;
      free(imageInfoPtr);
   }
   QThread::currentThread()->exit();
}

static LONG __cdecl fileCallback(CONST LPSTR path)
{
   LONG status = 0;

   return status;
}

/*********************************
 * High level utility functions. *
 *********************************/

LONG AspectDetector::readIniFiles(QString aspectFilename, QString detectorFilename)
{
   LONG status = -1;

   status = 0;
   iniFile = new IniFile(aspectFilename);
   directory = iniFile->getString("Controls/Toplevel Path");
   prefix = iniFile->getString("Controls/Prefix");
   detectorIniFile = new IniFile(detectorFilename);

   return status;
}

void AspectDetector::connectUp()
{
   showImageEvent = new WindowsEvent(HEXITEC_SHOW_IMAGE, true);
   startTriggerEvent = new WindowsEvent(HEXITEC_START_TRIGGER, true);
   triggeredEvent = new WindowsEvent(HEXITEC_TRIGGERED, true);
   triggeredReducedDataEvent = new WindowsEvent(HEXITEC_TRIGGERED_REDUCED_DATA, true);
   stopEvent = new WindowsEvent(HEXITEC_STOP, true);
   saveBufferEvent = new WindowsEvent(HEXITEC_SAVE_BUFFER, true);
   stopGrabEvent = new WindowsEvent(HEXITEC_STOP_GRAB, true, true);

   showImageEvent->connect1(this, SLOT(handleShowImage()));
   triggeredEvent->connect1(this, SLOT(handleTriggered()));
   triggeredReducedDataEvent->connect1(this, SLOT(handleTriggeredReducedData()));
   stopEvent->connect1(this, SLOT(handleStop()));

   connect(this, SIGNAL(executeGetImages()), this, SLOT(handleExecuteGetImages()));
}

int AspectDetector::configure(int mode)
{
   LONG status = -1;
   setMode((AspectDetector::Mode) mode);

   if (connected)
   {
      status = terminateConnection();
   }

   /* A delay seems necessary to stop a hang in the initialiseConnection() call.
    * Hang happens in the InitFrameGrabber dll call.
    * Half a second seems to be enough and shouldn't be noticeable. */
   Sleep(500);

   status = initialiseConnection();
   emit notifyMode(this->mode);

   return status;
}

int AspectDetector::getImages(int count, int ndaq)
{
   this->count = count;
   LONG status = -1;

   //   qDebug() << "GetImages: thread id " << QThread::currentThreadId();

   if (sensorMode == REDUCED_DATA)
   {
      //      qDebug() << "GetImages: in reduced data mode, first run after init = " << firstRunAfterInit;
      setGetImageParams();

      if (ndaq == 0 && offsetsOn)
      {
         updateState(OFFSETS_PREP);
         emit prepareForOffsets();
      }
      else
      {
         handleReducedDataCollection();
      }
   }
   else /* Sensor mode Analogue. */
   {
      this->count = count;
      streaming = 0;
      sprintf_s(pathString, "");

      imgCntAverage = 1;
      zeroSummedImageDest();
      emit executeGetImages();
   }

   return status;
}

void AspectDetector::setGetImageParams()
{
   QString path;

   streaming = 1;
   path = directory + "/" + prefix;

   if (timestampOn)
   {
      path += QDateTime::currentDateTime().toString("yyMMdd_hhmmss") + "_reduced";
   }

   path.replace(QString("/"), QString("\\"));
   sprintf_s(pathString, "%s", path.toUtf8().data());

   imgCntAverage = 1;
}

void AspectDetector::getOffsets()
{
   setGetImageParams();
   if (offsetsOn)
   {
      updateState(OFFSETS_PREP);
      emit prepareForOffsets();
   }

}

void AspectDetector::handleExecuteOffsets()
{
   if (offsetsOn)
   {
      updateState(OFFSETS);
      collectOffsets();
      updateState(COLLECTING_PREP);
      emit prepareForDataCollection();
   }
   else
   {
      updateState(READY);
   }
}

void AspectDetector::handleReducedDataCollection()
{
   restartReducedDataMode();
   emit executeGetImages();
}

void AspectDetector::handleExecuteGetImages()
{
   int status;
   int mult = detectorIniFile->getInt("Detector/Frame Update");

   if (sensorMode == REDUCED_DATA)
   {
      count = -1;
   }

   /* Save to disk in asynchronous mode.
      Use triggering. */
   status = GetImages(detectorHandle,
                      imageDest,     // Image Destination
                      imageInfoPtr,  // Image Destination Info
                      triggered,     // useTrigger, 0 = disable, 1 = enable
                      streaming,     // enable streaming straight to disc, 0 = disable, 1 = enable
                      imgCntAverage, // imgCntAverage, number of raw images to be averaged, reduced data = 1 analogue sensor = 1....
                      0,             // Number of images before trigger event. If analogue mode
                      //                Streaming direct to disc 0....imgCntMaxFrameBuffer/imgCntAverage.
                      //                No streaming imgCntPreTrg+imgCntPostTrg<=imgCntFrameBuffer/imgCntAverage.
                      //                Reduced data = 0.
                      count*mult,    // Number of images after trigger event, -1 => unitil error or user abort.
                      //                If streaming direct to disc enabled 0...(2^31)-1. imgCntPostTrg<=10^8*frames per file
                      //                If streaming direct to disc disabled imgCntPreTrg+imgCntPostTrg<=imgCntMaxFrameBuffer/imgCntAverage
                      //                If asynchronous and file saving disabled or streaming direct to disc enabled -1=acquiring until user about or error
                      pathString,    // Path including prefix for saving acquired images. Zero length string disables.
                      1,             // Remote synchronous = 0 asynchronous = 1
                      0              // Ignored for asynchronous. synchronous >= maxTimeToTriggerEvent+frameTime*imgCntPostTrg*ingCntAverage+savingTime
                      );
   analyseErrorStatus(status, "GetImages");

   if (mode == SOFT_TRIGGER || mode == EXTERNAL_TRIGGER)
   {
      updateState(WAITING_TRIGGER);
   }
   else
   {
      updateState(COLLECTING);
      if (mode == REDUCED_DATA)
      {
         startReducedDataStateMachine();
      }
   }

   if (mode == CONTINUOUS)
   {
      if (dataAcquisitionDuration > 0.0)
      {
         getImagesTimerEventId = QObject::startTimer(dataAcquisitionDuration);
         startTime = QDateTime::currentMSecsSinceEpoch();
      }
   }
}

void AspectDetector::setMode(Mode mode)
{
   this->mode = mode;
}

bool AspectDetector::isReducedDataMode(Mode mode)
{
   bool isReducedDataMode = false;

   if (mode == AspectDetector::CONTINUOUS || mode == AspectDetector::SOFT_TRIGGER  || mode == AspectDetector::EXTERNAL_TRIGGER)
   {
      isReducedDataMode = true;
   }

   return isReducedDataMode;
}

void AspectDetector::setQuadrant(int quadrant)
{
   this->quadrant = quadrant;
}

void AspectDetector::setDirectory(QString directory)
{
   this->directory = directory;
}

void AspectDetector::setDataAcquisitionDuration(double dataAcquisitionDuration)
{
   this->dataAcquisitionDuration = dataAcquisitionDuration;
}

QString AspectDetector::getDirectory()
{
   return this->directory;
}

void AspectDetector::setPrefix(QString prefix)
{
   this->prefix = prefix;
}

QString AspectDetector::getPrefix()
{
   return prefix;
}

void AspectDetector::setTimestampOn(bool timestampOn)
{
   //qDebug() << "AspectDetector Setting timestampOn to " << timestampOn;
   this->timestampOn = timestampOn;
}

bool AspectDetector::getTimestampOn()
{
   return timestampOn;
}

AspectDetector::Mode AspectDetector::getMode()
{
   return mode;
}

QStringList AspectDetector::getModes()
{
   return modes;
}

QStringList AspectDetector::getReducedDataModes()
{
   return reducedDataModes;
}

void AspectDetector::enableDarks()
{
   offsetsOn = true;
}

void AspectDetector::disableDarks()
{
   offsetsOn = false;
}

QSize AspectDetector::getSize()
{
   return QSize(xRes, yRes);
}

int AspectDetector::abort(bool restart)
{
   int status = 0;
   int reg2A;

   if (((mode == SOFT_TRIGGER) || (mode == EXTERNAL_TRIGGER)) && (state == WAITING_TRIGGER))
   {
      startReducedDataStateMachine();
   }

   SetEvent(stopGrabEvent->handle);

   // Added clear bit 6 reg2A as in flowchart 3.9.4
   if (sensorMode == REDUCED_DATA)
   {
      getFPGARegister("2A", &reg2A);
      reg2A &= 0xBF;
      writeFPGARegister("2A", twoCharHexNumber(reg2A).toUtf8().data()); // Clear bit 6
   }

   waitForEvent(stopEvent);
   ResetEvent(stopGrabEvent->handle);
   updateState(READY);
   firstRunAfterInit = false;

   if (restart && ((mode == SOFT_TRIGGER)|| (mode == EXTERNAL_TRIGGER)))
   {
      setGetImageParams();
      restartReducedDataMode();
      emit executeGetImages();
   }

   return status;
}

int AspectDetector::triggerCollection()
{
   int status = 0;

   if (mode == SOFT_TRIGGER)
   {
      startReducedDataStateMachine();
   }
   updateState(COLLECTING);

   if (dataAcquisitionDuration > 0.0)
   {
//      getImagesTimerEventId = QObject::startTimer(dataAcquisitionDuration * 1000);
      getImagesTimerEventId = QObject::startTimer(dataAcquisitionDuration);
      startTime = QDateTime::currentMSecsSinceEpoch();
   }

   return status;
}

QString AspectDetector::getState(DetectorState state)
{
   QString stateName;

   switch (state)
   {
   case INITIALISING:
      stateName = QString("Initialising");
      break;
   case INITIALISED:
      stateName = QString("Initialised");
      break;
   case READY:
      stateName = QString("Ready");
      break;
   case WAITING_DARK:
      stateName = QString("Waiting for darkness");
      break;
   case OFFSETS_PREP:
      stateName = QString("Preparing for offsets");
      break;
   case OFFSETS:
      stateName = QString("Collecting offsets");
      break;
   case COLLECTING_PREP:
      stateName = QString("Preparing for data collection");
      break;
   case COLLECTING:
      stateName = QString("Collecting data");
      break;
   case WAITING_TRIGGER:
      stateName = QString("Waiting for trigger");
      break;
   default:
      stateName = QString("Unknown");
      break;
   }

   return stateName;
}

void AspectDetector::updateState(DetectorState state)
{
   this->state = state;
   emit notifyState(state);
}

void AspectDetector::emitSensorType(int sensorType)
{
   switch (sensorType)
   {
   case SENSOR_20x20:
      emit writeMessage("Sensor type 20x20");
      break;
   case SENSOR_80x80:
      emit writeMessage("Sensor type 80x80");
      break;
   case SENSOR_80x80_2x2:
      emit writeMessage("Sensor type 80x80 (2x2)");
      break;
   default:
      emit writeMessage("Unsupported sensor type");
      break;
   }
}

QString AspectDetector::getAspectIniFilename()
{
   return aspectFilename;
}

QString AspectDetector::getDetectorIniFilename()
{
   return detectorFilename;
}

/********************************
 * Low level utility functions. *
 ********************************/

int AspectDetector::initialiseConnection()
{
   int version;
   int sensorType;
   int reg0;
   LONG asError = -1;
   LONG niError = -1;
   LONG sysError = -1;
   LONG status;
   char *icdFile;

   updateState(INITIALISING);
   idx = 0;
   reducedDataTimeout = iniFile->getInt("Controls/Reduced Data Timeout (ms)");
   connected = false;
   triggered = false;

   switch (mode)
   {
   case FIXED:
      sensorMode = ANALOGUE;
      icdFile = "Aspect_Hexitec.icd";
      break;
   case CONTINUOUS:
   case SOFT_TRIGGER:
   case EXTERNAL_TRIGGER:
      sensorMode = REDUCED_DATA;
      icdFile = "Aspect_Hexitec Reduced Data.icd";
      break;
   }

   status = InitFrameGrabber(&detectorHandle,
                             "img0",
                             icdFile,
                             &asError,
                             &niError,
                             &sysError);

   if (asError != 0 ||
       niError != 0 ||
       sysError != 0 ||
       detectorHandle == NULL)
   {
      connected = false;
      emit writeError("Failed to initialise Aspect detector frame grabber.");
      analyseErrorStatus(status, "InitFrameGrabber");
      analyseErrorStatus(asError, niError, sysError);
   }
   /* TO DO: Deal with status properly. Perhaps use exceptions rather than testing each time. */
   else
   {
      emit writeMessage("Aspect detector frame grabber initialised using icd file " + QString::fromUtf8(icdFile));

      status = initialiseModules();
      analyseErrorStatus(status, "initialiseModules");

      status = firmwareVersion(&version);
      if (version < MINIMUM_FIRMWARE_VERSION)
      {
         emit writeError("Firmware version error");
      }

      status = getFPGARegister("1E", &sensorType);
      sensorType &= (SENSOR_20x20 | SENSOR_80x80 | SENSOR_80x80_2x2);
      emitSensorType(sensorType);

      if (sensorType == SENSOR_80x80_2x2)
      {
         init_80x80_2x2_Sensor();
      }

      writeADC1Register("16", "03");
      writeADC2Register("16", "03");

      initFPGARegisters();
      writeDACConfiguration();

      getFPGARegister("00", &reg0);
      reg0 |= 0x01;
      writeFPGARegister("00", twoCharHexNumber(reg0).toUtf8().data()); // Clear bit 0

      getFPGARegister("1C", &xRes);
      getFPGARegister("1D", &yRes);

      writeCamLinkRegister();

      getFPGARegister("1E", &reg0);

      status = configureFrameGrabber();
      analyseErrorStatus(status, "configureFrameGrabber");

      firstRunAfterInit = true;
   }

   updateState(INITIALISED);
   updateState(READY);

   return status;
}

LONG AspectDetector::configureFrameGrabber()
{
   LONG status;
   ULONG imgCntMaxFrameBuffer; // Max Frame Buffers for Streaming to RAM
   int reg0;
   int reg1E;
   int xStartROI;
   int yStartROI;
   int xROISize;
   int yROISize;
   int imagesPerFile;
   int frameUpdate;

   emit writeMessage("Configuring Aspect detector frame grabber.");

   switch (mode)
   {
   case FIXED:
      frameUpdate = detectorIniFile->getInt("Detector/Frame Update"); // Adjust to get an image rate display can deal with
      xResAcquiredImage = xRes * 3;
      yResAcquiredImage = yRes;  //TODO: Should this be *3 (awaiting answer from Frank)
      xStartROI = 0;
      yStartROI = 0;
      xROISize = xResAcquiredImage;
      yROISize = yResAcquiredImage;
      frameMode = ANALOGUE_1_FRAME_DATA;
      sensorMode = ANALOGUE;
      if (imageInfoPtr == NULL)
      {
         imageInfoPtr = (ImageInfoPtr) malloc(sizeof(ImageInfo));
      }
      imageInfoPtr->border = 0;
      imageInfoPtr->imageType = AS_IMAGE_I16;
      imageInfoPtr->pixelSize = AS_PIXELSIZE_I16;
      imageInfoPtr->xRes = xResAcquiredImage;
      imageInfoPtr->yRes = yResAcquiredImage;
      imagesPerFile = iniFile->getInt("Controls/Images/File");
      imageSize = xResAcquiredImage * yResAcquiredImage;
      //imageSize = xRes * yRes;
      imageDest = (short *) malloc(imageSize * sizeof(short));
      summedImageDest = (int *) malloc(imageSize * sizeof(int));
      charImageDest = (unsigned char *) malloc(imageSize * sizeof(unsigned char));
      imageInfoPtr->pixelsPerLine = xResAcquiredImage;
      break;
   case CONTINUOUS:
   case SOFT_TRIGGER:
   case EXTERNAL_TRIGGER:
      frameUpdate = 1;
      xResAcquiredImage = 1024;
      yResAcquiredImage = 1024;
      xStartROI = 0;
      yStartROI = 0;
      xROISize = xResAcquiredImage;
      yROISize = yResAcquiredImage;
      frameMode = REDUCED_FRAME_DATA;
      sensorMode = REDUCED_DATA;
      imagesPerFile = iniFile->getInt("Controls/Number of images per file");
      imageSize = 0;
      imageDest = NULL;
      summedImageDest = NULL;
      charImageDest = NULL;
      free(imageInfoPtr);
      imageInfoPtr = NULL;
      break;
   }
   //qDebug() << "Controls/Number of images per file: " << imagesPerFile;

   getFPGARegister("00", &reg0);
   getFPGARegister("1E", &reg1E);
   if ((reg0 & 0x01) && (reg1E & 0x01))
   {
      emit writeMessage("Connection to aSpect detector opened with sensor mode " + QString::number(sensorMode));
      connected = true;
      status = ConfigureFrameGrabber(detectorHandle, // Detector Handle
                                     iniFile->getInt("Controls/Trigger Polarity"),  // Trigger Polarity, 0 or 1
                                     imageDest,                                     // Image Destination
                                     imageInfoPtr,                                  // Image Destination Info
                                     "",                                            // Image Correction Path
                                     "",                                            // Image Dark Path
                                     0,                                             // Do Correct Image, 0 or 1
                                     0,                                             // Do Dark Image, 0 or 1
                                     xResAcquiredImage,                             // X Resolution Of Acquired Image
                                     yResAcquiredImage,                             // Y Resolution Of Acquired Image
                                     xStartROI,                                     // Start of ROI in X
                                     yStartROI,                                     // Start of ROI in Y
                                     xROISize,                                      // Size of ROI in X
                                     yROISize,                                      // Size of ROI in Y
                                     frameUpdate,                                   // Frame Update, nth Image
                                     sensorMode,                                    // 0 = Analogue, 1 = Reduce Data
                                     imagesPerFile,                                 // Two ini file parameters are available.
                                     // In analoge mode - use "Images/File"
                                     // In reduced data mode - use "Number of images per file"
                                     reducedDataTimeout,                            // Timeout
                                     iniFile->getInt("Controls/Number of buffers"), // Number of Buffers For Image Acquisition
                                     &imgCntMaxFrameBuffer);

      analyseErrorStatus(status, "ConfigureFrameGrabber");
   }
   else
   {
      // TO DO : Deal with error correctly. Message. Don't go on to do other things...
   }

   return status;
}

int AspectDetector::terminateConnection()
{
   LONG status = -1;
   int sensorType;

   status = getFPGARegister("1E", &sensorType);
   sensorType &= (SENSOR_20x20 | SENSOR_80x80 | SENSOR_80x80_2x2);
   emitSensorType(sensorType);

   if (sensorType == SENSOR_80x80_2x2)
   {
      terminate_80x80_2x2_Sensor();
   }

   exitModules();
   status = ExitFrameGrabber(detectorHandle);
   analyseErrorStatus(status, "ExitFrameGrabber");

   if (imageDest != NULL)
   {
      free(imageDest);
   }
   if (summedImageDest != NULL)
   {
      free(summedImageDest);
   }
   if (charImageDest != NULL)
   {
      free(charImageDest);
   }
   imageSize = 0;

   return status;
}

LONG AspectDetector::initialiseModules()
{
   LONG status = -1;
   // TO DO: check status each time and return properly.

   emit writeMessage("Initialize FPGA ");
   initFPGA();
   emit writeMessage("Initialize DAC ");
   initDAC();
   emit writeMessage("Initialize ADC1 ");
   initADC1();
   emit writeMessage("Initialize ADC2 ");
   initADC2();

   status = 0;

   return status;
}

LONG AspectDetector::exitModules()
{
   LONG status = -1;
   // TO DO: check status each time and return properly.

   emit writeMessage("Exit FPGA ");
   exitFPGA();
   emit writeMessage("Exit DAC ");
   exitDAC();
   emit writeMessage("Exit ADC1 ");
   exitADC1();
   emit writeMessage("Exit ADC2 ");
   exitADC2();

   status = 0;

   return status;
}

void AspectDetector::imageDestToPixmap()
{
   QVector<QRgb> colorTable;
   int xSize;

   for (int i = 0; i < 256; i++)
   {
      colorTable.push_back(QColor(i, i, i).rgb());
   }

   if (imageDest != NULL)
   {
      for (int i = 0; i <= 3; i++)
      {
         charImageDest = getImage(i); // TO DO : Free the memory malloc'd
         switch (i)
         {
         case 0:
            xSize = xRes * 3;
            break;
         default:
            xSize = xRes;
            break;
         }
         QImage image(charImageDest, xSize, yRes, QImage::Format_Indexed8);

         image.setColorTable(colorTable);
         imagePixmap = QPixmap::fromImage(image);

         switch (i)
         {
         case 0:
            emit imageAcquired(imagePixmap);
            break;
         case 1:
            emit image1Acquired(imagePixmap);
            break;
         case 2:
            emit image2Acquired(imagePixmap);
            break;
         case 3:
            emit image3Acquired(imagePixmap);
            break;
         }
      }
   }
}

unsigned char *AspectDetector::getImage(int imageNumber)
{
   int segmentSize = xRes * yRes;
   int offset = (imageNumber - 1) * segmentSize;
   unsigned char *image;
   short current, min = 32767, max = -32768, range;
   int sum = 0, mean;

   if (imageNumber == 0)
   {
      segmentSize *= 3;
      offset = 0;
   }

   image = (unsigned char *) malloc(segmentSize * sizeof(unsigned char));

   for (int i = offset; i < offset + segmentSize; i++)
   {
      summedImageDest[i] += imageDest[i];
      current = imageDest[i];
      sum += current;
      if (current > max)
      {
         max = current;
      }
      if (current < min )
      {
         min = current;
      }
   }
   range = max - min;
   mean = sum / segmentSize;
   //emit writeMessage("Image " + QString::number(imageNumber) + " min = " + QString::number(min) + " max " + QString::number(max) + " mean = " + QString::number(mean));
   switch (range)
   {
   case 0:
      for (int i = 0; i < segmentSize; i++)
      {
         image[i] = 0;
      }
      break;
   default:
      for (int i = 0; i < segmentSize; i++)
      {
         image[i] = ((imageDest[i + offset] - min) * 255) / range;
      }
      break;
   }

   return image;
}

void AspectDetector::writeImages()
{
   QFile file("U:/DSoFt_Images/image.raw");

   file.open(QIODevice::WriteOnly);
   QDataStream out(&file);
   for (int i = 0; i < xResAcquiredImage * yResAcquiredImage; i++)
   {
      out << imageDest[i];
   }
   file.close();
}

void AspectDetector::zeroSummedImageDest()
{
   memset((void *) summedImageDest, 0, sizeof(int) * imageSize);
}

int AspectDetector::waitForEvent(WindowsEvent *event)
{
   return waitForEvent(event->handle, event->name);
}

int AspectDetector::waitForEvent(HANDLE handle, QString name)
{
   int status = -1;
   DWORD eventStatus;

   eventStatus = WaitForSingleObject(handle, 2000);
   switch (eventStatus)
   {
   case WAIT_OBJECT_0:
      status = 0;
      emit writeMessage("Event " + QString(name) + " received");
      break;
   case WAIT_TIMEOUT:
      emit writeError("TIMED OUT in wait for " + QString(name) + " event");
      break;
   case WAIT_ABANDONED:
      emit writeError("ABANDONED wait for " + QString(name) + " event");
      break;
   case WAIT_FAILED:
      emit writeError("Wait for " + QString(name) + " event FAILED.");
      break;
   }

   return status;
}

void AspectDetector::analyseErrorStatus(long status, QString location)
{
   if (status != 0)
   {
      emit writeMessage(location + " returned ERROR status = " + QString::number(status));
      //qDebug() << location + " returned ERROR status = " + QString::number(status);
      getLastErrorString(status, 0, 0);
   }
}

void AspectDetector::analyseErrorStatus(long asErr, long niErr, long sysErr)
{
   if (asErr != 0 ||
       niErr != 0 ||
       sysErr != 0)
   {
      getLastErrorString(asErr, niErr, sysErr);
   }
}

double AspectDetector::calculateFrameTime()
{
   int rowS1;
   int s1sph;
   int sphs2;
   double rowsPerFrame = 80;
   double fPixel = 20;
   double frameTime;

   rowS1 = iniFile->getInt("Controls/Row -> S1");
   s1sph = iniFile->getInt("Controls/S1 -> Sph");
   sphs2 = iniFile->getInt("Controls/Sph -> S2");

   frameTime = double(rowS1 + s1sph + sphs2 + 100) * rowsPerFrame / fPixel;
   /*qDebug() << "\nFrame Time = (" << QString::number(rowS1)<< " + " << QString::number(s1sph)<< " + " <<QString::number(sphs2)<< " + 100) * "
            <<QString::number(rowsPerFrame)<< " / " <<QString::number(fPixel)<< " = " <<QString::number(frameTime);*/

   return frameTime/1000;
}

void AspectDetector::getLastErrorString(long asErr, long niErr, long sysErr)
{
   char asErrStr[ASPECT_LEN_ERR_MSG];
   char niErrStr[ASPECT_LEN_ERR_MSG];
   char sysErrStr[ASPECT_LEN_ERR_MSG];
   char message[ASPECT_LEN_ERR_MSG * 3];

   QString stringMessage;

   GetLastErrorStrA( &asErr, asErrStr,
                     &niErr, niErrStr,
                     &sysErr, sysErrStr,
                     ASPECT_LEN_ERR_MSG);
   if (asErr != 0)
   {
      sprintf_s(message, "aSpect err [%ld]: %s\n", asErr, asErrStr);
      stringMessage = QString::fromUtf8(message);
      emit writeError(stringMessage);
   }

   if (niErr != 0)
   {
      sprintf_s(message, "aSpect NI err [%ld]: %s\n", niErr, niErrStr);
      stringMessage = QString::fromUtf8(message);
      emit writeError(stringMessage);
   }

   if (sysErr != 0)
   {
      sprintf_s(message, "aSpect WIN err [%ld]: %s\n", sysErr, sysErrStr);
      stringMessage = QString::fromUtf8(message);
      emit writeError(stringMessage);
   }
}

unsigned int AspectDetector::reverseBits(unsigned int num)
{
   unsigned int NO_OF_BITS = sizeof(num) * 8;
   unsigned int reverse_num = 0;
   unsigned int i;

   for (i = 0; i < NO_OF_BITS; i++)
   {
      if ((num & (1 << i)))
      {
         reverse_num |= 1 << ((NO_OF_BITS - 1) - i);
      }
   }

   return reverse_num;
}

/****************************
 * Hardware access via DLL. *
 ****************************/

LONG AspectDetector::breakConnection()
{
   LONG status = 0;

   writeFPGARegister("00", "00");

   return status;
}

LONG AspectDetector::collectOffsets()
{
   LONG status = -1;
   float sleepTime;
   int reg0;
   int reg2A;
   int reg1E;
   int reg;

   if (!firstRunAfterInit)
   {
      getFPGARegister("00", &reg0);
      reg0 &= 0xFE;
      writeFPGARegister("00", twoCharHexNumber(reg0).toUtf8().data()); // Clear bit 0

      getFPGARegister("00", &reg0);
      reg0 &= 0xFD;
      writeFPGARegister("00", twoCharHexNumber(reg0).toUtf8().data()); // Clear bit 1

      getFPGARegister("2A", &reg2A);
      reg2A |= 0x40;
      writeFPGARegister("2A", twoCharHexNumber(reg2A).toUtf8().data()); // Set bit 6

      getFPGARegister("00", &reg0);
      reg0 |= 0x01;
      writeFPGARegister("00", twoCharHexNumber(reg0).toUtf8().data()); // Set bit 0

      getFPGARegister("1E", &reg1E);
      if (reg1E && 0x01)
      {
         writeCamLinkRegister();
      }
   }

   getFPGARegister("2A", &reg2A);
   reg2A &= 0xBF;
   writeFPGARegister("2A", twoCharHexNumber(reg2A).toUtf8().data()); // Clear bit 6

   // Wait for frame time * 8192.
   sleepTime = frameTime * 8192;
   Sleep((int)sleepTime);

   status = getFPGARegister("0D", &reg);

   getFPGARegister("1E", &reg1E);
   if (reg1E && 0x04)
   {
      emit writeMessage("Offsets collection succeeded.");
      status = 0;
   }
   else
   {
      emit writeError("Offsets collection FAILED.");
   }

   return status;
}


LONG AspectDetector::restartReducedDataMode()
{
   LONG status = -1;
   int reg0;
   int reg2A;
   int reg0D;
   QTime time;

   getFPGARegister("2A", &reg2A);
   reg2A &= 0xBF;
   writeFPGARegister("2A", twoCharHexNumber(reg2A).toUtf8().data()); // Clear bit 6

   getFPGARegister("00", &reg0);
   reg0 |= 0x02;
   writeFPGARegister("00", twoCharHexNumber(reg0).toUtf8().data()); // Set bit 1

   getFPGARegister("00", &reg0);
   reg0 &= 0xFD;
   writeFPGARegister("00", twoCharHexNumber(reg0).toUtf8().data()); // Clear bit 1

   time.start();
   while ((time.elapsed() < 3000) && (status != 0))
   {
      getFPGARegister("0D", &reg0D);
      if (reg0D && 0x03)
      {
         status = 0;
      }
      Sleep(100);
   }

   return status;
}

LONG AspectDetector::startReducedDataStateMachine()
{
   LONG status = -1;
   int reg2A;

   getFPGARegister("2A", &reg2A);
   reg2A |= 0x40;
   writeFPGARegister("2A", twoCharHexNumber(reg2A).toUtf8().data()); // Set bit 6

   status = 0;

   return status;
}

LONG AspectDetector::initFPGA()
{
   LONG status;
   ULONG length;

   length = 4;
   status = serialWrite((CONST PUCHAR) "\x23\x30\xE3\x0D", &length);

   return status;
}

LONG AspectDetector::exitFPGA()
{
   LONG status;
   ULONG length;

   length = 4;
   status = serialWrite((CONST PUCHAR) "\x23\x30\xE2\x0D", &length);

   return status;
}

LONG AspectDetector::initCameraLink()
{
   LONG status;
   ULONG length;

   length = 4;
   status = serialWrite((CONST PUCHAR) "\x23\x40\xE3\x0D", &length);

   return status;
}

LONG AspectDetector::exitCameraLink()
{
   LONG status;
   ULONG length;

   length = 4;
   status = serialWrite((CONST PUCHAR) "\x23\x40\xE2\x0D", &length);

   return status;
}

LONG AspectDetector::initDAC()
{
   LONG status;
   ULONG length;

   length = 4;
   status = serialWrite((CONST PUCHAR) "\x23\x50\xE3\x0D", &length);

   return status;
}

LONG AspectDetector::exitDAC()
{
   LONG status;
   ULONG length;

   length = 4;
   status = serialWrite((CONST PUCHAR) "\x23\x50\xE2\x0D", &length);

   return status;
}

LONG AspectDetector::initADC1()
{
   LONG status;
   ULONG length;

   length = 4;
   status = serialWrite((CONST PUCHAR) "\x23\x60\xE3\x0D", &length);

   return status;
}

LONG AspectDetector::exitADC1()
{
   LONG status;
   ULONG length;

   length = 4;
   status = serialWrite((CONST PUCHAR) "\x23\x60\xE2\x0D", &length);

   return status;
}

LONG AspectDetector::initADC2()
{
   LONG status;
   ULONG length;

   length = 4;
   status = serialWrite((CONST PUCHAR) "\x23\x61\xE3\x0D", &length);

   return status;
}

LONG AspectDetector::exitADC2()
{
   LONG status;
   ULONG length;

   length = 4;
   status = serialWrite((CONST PUCHAR) "\x23\x61\xE2\x0D", &length);

   return status;
}


QString AspectDetector::twoCharHexNumber(int number)
{
   char buffer[3];
   sprintf_s(buffer, "%02x", number);
   //QString s = QString::number(number);
   return QString(buffer);
}

LONG AspectDetector::init_80x80_2x2_Sensor()
{
   LONG status = -1;
   int reg;

   // Clear bit 0 of register 0C
   getFPGARegister("0C", &reg);
   reg &= 0xFE;
   writeFPGARegister("0C", twoCharHexNumber(reg).toUtf8().data());

   //qDebug() << "init_80x80_2x2_Sensor quadrant " << this->quadrant;
   /* Write register 0x0B. This selects which Sensor will be powered up during initialisation.
    * Email Frank Lauba 18/06/13. */
   writeFPGARegister("0B", twoCharHexNumber(this->quadrant).toUtf8().data());

   // Set bit 0 of register 0C
   getFPGARegister("0C", &reg);
   reg |= 0x01;
   writeFPGARegister("0C", twoCharHexNumber(reg).toUtf8().data());

   Sleep(2500);

   status = 0;

   return status;
}

LONG AspectDetector::terminate_80x80_2x2_Sensor()
{
   LONG status = -1;
   status = 0;

   /* Reset bit 0 of register 0x0C (Activate Selected Sensor).
    * Setting to 0 1 de-activates selection. */
   writeFPGARegister("0C", "00");
   /* Set 0B (sensor select) to 0 - disables sensors. */
   writeFPGARegister("0B", "00");
   /* Set bit 0 of register 0x0C (Activate Selected Sensor).
    * Changing from 0 -> 1 activates selection. Make no sensors selected (I think)*/
   writeFPGARegister("0C", "01");

   return status;
}

LONG AspectDetector::setRegisterBlock(int startReg, QString block)
{
   LONG status = -1;
   QString section;
   bool ok;
   unsigned int sectionInt;
   int reg = startReg;
   QString sectionHex;
   QString registerHex;
   int leadingZeros;

   for (int i = 0; i < 80; i+=8)
   {
      section = block.mid(i, 8);
      sectionInt = section.toInt(&ok, 2);
      if (ok)
      {
         sectionInt = reverseBits(sectionInt);
         registerHex = QString::number(reg++, 16);
         sectionHex = QString::number(sectionInt, 16);
         leadingZeros = 8 - sectionHex.length();
         for (int i = 0; i < leadingZeros; i++)
         {
            sectionHex.prepend('0');
         }
         writeFPGARegister(registerHex.toUtf8().data(), sectionHex.toUtf8().data());
         //emit writeMessage("Section " + number(i) + " = " + section + " converts to hex = " + sectionHex);
         //emit writeMessage("Writing register " + registerHex);
         //qDebug() << "registerHex" << registerHex << "section" << section << "sectionInt" << sectionInt << "sectionHex" << sectionHex << "length" << sectionHex.length();
      }
   }
   status = 0;

   return status;
}

LONG AspectDetector::initRegisterBlock(int startReg, QString nameStart)
{
   LONG status = -1;

   QString blockName = nameStart;
   QString block;
   int length = rowNameEnds.length();

   for (int i=0; i<length; i++)
   {
      if (nameStart.contains("Row"))
      {
         blockName = nameStart + rowNameEnds.at(i);
      }
      else if (nameStart.contains("Column"))
      {
         blockName = nameStart + columnNameEnds.at(i);
      }
      block += iniFile->getString(blockName);
      //emit writeMessage("BLOCK NAME: " + blockName + " BLOCK " + block);
   }
   //emit writeMessage("Register block " + block);
   setRegisterBlock(startReg, block);

   status = 0;

   return status;
}

LONG  AspectDetector::initFPGARegisters()
{
   LONG status = -1;
   int registerZero;
   int register2A;
   int gain;
   int delay;
   int rowS1, rowS1LowByte, rowS1HighByte;
   int s1sph;
   int sphs2;
   //   int readOutChannel;
   int adc1Delay;
   int adc2Delay;
   int vcal2vcal1, vcal2vcal1LowByte, vcal2vcal1HighByte;
   int vcalTimeX, vcalTimeXLowByte, vcalTimeXHighByte;
   int vcalTimeY, vcalTimeYLowByte, vcalTimeYHighByte;
   int alpha;
   int beta;
   float fCutOff;
   int cutOff, cutOffLowByte, cutOffHighByte;
   int regValue;
   CHAR buffer[3];

   /* Set register 01 (Bits 0..4 delay, bit 5 gain (0 = High, 1 = Low)) */
   gain = iniFile->getInt("Controls/Gain");
   delay = iniFile->getInt("Controls/delay sync signals");
   regValue = ((gain<<4) | delay);
   writeFPGARegister("01", twoCharHexNumber(regValue).toUtf8().data());

   rowS1 = iniFile->getInt("Controls/Row -> S1");
   rowS1LowByte = rowS1 & 0xFF;
   rowS1HighByte = ((rowS1 & 0x3F00) >> 8);
   /* Set register 02 (0..7) Low byte RowS1. */
   writeFPGARegister("02", twoCharHexNumber(rowS1LowByte).toUtf8().data());
   /* Set register 03 (0..5) High byte RowS1. */
   writeFPGARegister("03", twoCharHexNumber(rowS1HighByte).toUtf8().data());

   /* Set register 04 (0..5) S1Sph */
   s1sph = iniFile->getInt("Controls/S1 -> Sph");
   s1sph &= 0x3F;
   writeFPGARegister("04", twoCharHexNumber(s1sph).toUtf8().data());

   /* Set register 05 (0..5) SphS2 */
   sphs2 = iniFile->getInt("Controls/Sph -> S2");
   sphs2 &= 0x3F;
   writeFPGARegister("05", twoCharHexNumber(sphs2).toUtf8().data());

   /* Read Out Channel (0..3) - quadrant -1. for 20x20 always 0 */
   //qDebug() << "initFPGARegisters quadrant " << this->quadrant;
   writeFPGARegister("08", twoCharHexNumber(this->quadrant - 1).toUtf8().data());

   /* Set register 09 ADC1 Delay */
   adc1Delay = iniFile->getInt("Controls/ADC1 Delay");
   writeFPGARegister("09", twoCharHexNumber(adc1Delay).toUtf8().data());

   /* Set register 0A ADC2 Delay */
   adc2Delay = iniFile->getInt("Controls/ADC2 Delay");
   writeFPGARegister("0A", twoCharHexNumber(adc2Delay).toUtf8().data());

   vcal2vcal1 = iniFile->getInt("Controls/VCAL2 -> VCAL1");
   vcal2vcal1LowByte = vcal2vcal1 & 0xFF;
   vcal2vcal1HighByte = ((vcal2vcal1 & 0xFF00) >> 8);
   /* Set register 18 (0..7) Low byte vcal2 -> vcal1. */
   writeFPGARegister("18", twoCharHexNumber(vcal2vcal1LowByte).toUtf8().data());
   /* Set register 19 (0..7) High byte RowS1. */
   writeFPGARegister("19", twoCharHexNumber(vcal2vcal1HighByte).toUtf8().data());

   /* Set register 1A SM wait clock row always 0x0? */
   writeFPGARegister("1A", "00");
   /* Set register 1B SM wait clock row always 0x08? */
   writeFPGARegister("1B", "08");

   vcalTimeX = iniFile->getInt("Controls/VCAL time X");
   vcalTimeXLowByte = vcalTimeX & 0xFF;
   vcalTimeXHighByte = ((vcalTimeX & 0xFF00) >> 8);
   /* Set register 20 low byte SM X clock vcal */
   writeFPGARegister("20", twoCharHexNumber(vcalTimeXLowByte).toUtf8().data());
   /* Set register 21 high byte SM X clock vcal */
   writeFPGARegister("21", twoCharHexNumber(vcalTimeXHighByte).toUtf8().data());

   vcalTimeY = iniFile->getInt("Controls/VCAL time Y");
   vcalTimeYLowByte = vcalTimeY & 0xFF;
   vcalTimeYHighByte = ((vcalTimeY & 0xFF00) >> 8);
   /* Set register 20 low byte SM Y clock vcal */
   writeFPGARegister("22", twoCharHexNumber(vcalTimeYLowByte).toUtf8().data());
   /* Set register 20 low byte SM Y clock vcal */
   writeFPGARegister("23", twoCharHexNumber(vcalTimeYHighByte).toUtf8().data());

   alpha = iniFile->getInt("Controls/Alpha");
   /* Set register 24 Alpha (should this be alpha*100? alpha is 0 so not a problem now) */
   writeFPGARegister("24", twoCharHexNumber(alpha).toUtf8().data());

   beta = iniFile->getInt("Controls/Beta");
   /* Set register 25 Beta (should this be beta*100? beta is 0 so not a problem now) */
   writeFPGARegister("25", twoCharHexNumber(beta).toUtf8().data());

   fCutOff = iniFile->getFloat("Controls/CutOff");
   cutOff = int(fCutOff + 0.5);
   cutOffLowByte = cutOff & 0xFF;
   cutOffHighByte = ((cutOff & 0xFF00) >> 8);
   /* Set register 2B high low CutOff */
   writeFPGARegister("2B", twoCharHexNumber(cutOffLowByte).toUtf8().data());
   /* Set register 2C high byte CutOff */
   writeFPGARegister("2C", twoCharHexNumber(cutOffHighByte).toUtf8().data());

   initRegisterBlock(0x2F, "Controls/RowPwr");
   initRegisterBlock(0x39, "Controls/RowCal");
   initRegisterBlock(0x43, "Controls/RowEn_");
   initRegisterBlock(0x4D, "Controls/ColumnPwr");
   initRegisterBlock(0x57, "Controls/ColumnCal");
   initRegisterBlock(0x61, "Controls/ColumnEn_");

   writeRegister00();
   /* Set bits 0&1 of reg 0 to 0 */
   getFPGARegister("00", &registerZero);
   registerZero &= 0xFC;
   sprintf_s(buffer, "%02X", registerZero);
   writeFPGARegister("00", buffer);

   /* Write register 2A depending on sensor mode */
   getFPGARegister("2A", &register2A);
   switch (sensorMode)
   {
   case ANALOGUE:
      writeFPGARegister("2A", ANALOGUE_MODE);
      break;
   case REDUCED_DATA:
      writeFPGARegister("2A", REDUCED_DATA_MODE);
      break;
   }
   getFPGARegister("2A", &register2A);
   status = 0;

   return status;
}

void AspectDetector::writeRegister00()
{
   if (sensorMode == ANALOGUE)
   {
      writeFPGARegister("00", "04");
   }
   else if (sensorMode == REDUCED_DATA)
   {
      writeFPGARegister("00", "C4");
   }
}

LONG  AspectDetector::writeDACConfiguration()
{
   LONG status = -1;
   float urefMid, vcal1, vcal2;
   QString DACConfiguration(128, '0');

   urefMid = iniFile->getFloat("Controls/Uref_mid");
   vcal1 = iniFile->getFloat("Controls/VCAL 1");
   vcal2 = iniFile->getFloat("Controls/VCAL 2");

   calcDACChannelValue(vcal2, &DACConfiguration, 0);
   calcDACChannelValue(vcal1, &DACConfiguration, 1);
   calcDACChannelValue(urefMid, &DACConfiguration, 6);
   calcDACChannelValue(vcal2, &DACConfiguration, 7);
   calcDACChannelValue(vcal1, &DACConfiguration, 8);
   writeDACRegister(DACConfiguration);
   status = 0;

   return status;
}

LONG AspectDetector::firmwareVersion(int *version)
{
   return getFPGARegister("82", version);
}

LONG AspectDetector::firmwareProjectId()
{
   int value;
   return getFPGARegister("81", &value);
}

LONG AspectDetector::firmwareCustomerId()
{
   int value;
   return getFPGARegister("80", &value);
}

LONG AspectDetector::getRows()
{
   int value;
   return getFPGARegister("1D", &value);
}

LONG AspectDetector::getColumns()
{
   int value;
   return getFPGARegister("1C", &value);
}

LONG AspectDetector::getFPGARegister(CHAR *regNo, int *value)
{
   LONG status = 0;

   status = writeFPGARegister("1F", regNo);
   status = readRegister(FPGA_OFFSET, value);
   //emit writeMessage("Value of register " + QString(regNo) + " = " + QString::number(*value));

   return status;
}

LONG AspectDetector::getFPGARegister(CHAR *regNo, QString *value)
{
   LONG status = 0;
   int ivalue;

   getFPGARegister(regNo, &ivalue);

   *value = QString::number(ivalue);
   //emit writeMessage("Value of register " + QString(regNo) + " = " + *value);
   return status;
}

int AspectDetector::current(double *temperature)
{
   int status = 0;
   int lsb, msb;

   *temperature = 1.0;

   // Read temperature LSB from FPGA register 0x6E
   getFPGARegister("6E", &lsb);

   // Read temperature MSB from FPGA register 0x6F
   getFPGARegister("6F", &msb);

   // Calculate temperature [degC] = RegisterValue / 8 * 0.0626
   *temperature = (double)((msb<<8)+lsb) / 8.0 * 0.0626;

   return status;
}

LONG AspectDetector::writeFPGARegister(CHAR *registerNumber, CHAR *registerValue)
{
   LONG status;
   CHAR buffer[15]; // Max length from writing 8 Row/Column register blocks.
   ULONG readLength, writeLength;
   CHAR command;

   command = 0x40;

   writeLength = sprintf_s(buffer, "\x23%c%c%s%s\x0D", FPGA_OFFSET, command, registerNumber, registerValue);
   readLength = writeLength - 1;
   status = serialWrite((PUCHAR) buffer, &writeLength);
   QString writeLocation("writeFPGARegister (serialWrite)");
   writeLocation.append(buffer);
   analyseErrorStatus(status, writeLocation);
   //emit writeMessage("Bytes written = " + QString::number(writeLength));
   status = serialRead((PUCHAR) buffer, &readLength);
   QString readLocation("writeFPGARegister (serialRead)");
   readLocation.append(buffer);
   analyseErrorStatus(status, readLocation);

   return status;
}

LONG AspectDetector::writeCamLinkRegister()
{
   LONG status;
   CHAR buffer[5];
   ULONG readLength, writeLength;
   CHAR command;

   command = 0x42;
   readLength = 3;

   writeLength = sprintf_s(buffer, "\x23%c%c\x0D", CAMLINK_OFFSET, command);
   status = serialWrite((PUCHAR) buffer, &writeLength);
   QString writeLocation("writeCamLinkRegister (serialWrite)");
   writeLocation.append(buffer);
   analyseErrorStatus(status, writeLocation);
   //emit writeMessage("Bytes written = " + QString::number(writeLength));
   status = serialRead((PUCHAR) buffer, &readLength);
   QString readLocation("writeCamLinkRegister (serialRead)");
   readLocation.append(buffer);
   analyseErrorStatus(status, readLocation);

   return status;
}

LONG AspectDetector::writeDACRegister(QString registerValues)
{
   LONG status;
   CHAR buffer[133];
   ULONG readLength, writeLength;
   CHAR command;

   command = 0x43;
   readLength = 131;

   writeLength = sprintf_s(buffer, "\x23%c%c%s\x0D", DAC_OFFSET, command, registerValues.toUtf8().data());
   status = serialWrite((PUCHAR) buffer, &writeLength);
   QString writeLocation("writeDACRegister (serialWrite)");
   writeLocation.append(buffer);
   analyseErrorStatus(status, writeLocation);
   //emit writeMessage("Bytes written = " + QString::number(writeLength));
   status = serialRead((PUCHAR) buffer, &readLength);
   QString readLocation("writeDACRegister (serialRead)");
   readLocation.append(buffer);
   analyseErrorStatus(status, readLocation);

   return status;
}

LONG AspectDetector::writeADC1Register(CHAR *registerNumber, CHAR *registerValue)
{
   LONG status;
   CHAR buffer[132];
   ULONG readLength, writeLength;
   CHAR command;

   command = 0x41;
   readLength = 3;

   writeLength = sprintf_s(buffer, "\x23%c%c%s%s\x0D", ADC1_OFFSET, command, registerNumber, registerValue);
   status = serialWrite((PUCHAR) buffer, &writeLength);
   QString writeLocation("writeADC1Register (serialWrite)");
   writeLocation.append(buffer);
   analyseErrorStatus(status, writeLocation);
   //emit writeMessage("Bytes written = " + QString::number(writeLength));
   status = serialRead((PUCHAR) buffer, &readLength);
   QString readLocation("writeADC1Register (serialRead)");
   readLocation.append(buffer);
   analyseErrorStatus(status, readLocation);

   return status;
}

LONG AspectDetector::writeADC2Register(CHAR *registerNumber, CHAR *registerValue)
{
   LONG status;
   CHAR buffer[132];
   ULONG readLength, writeLength;
   CHAR command;

   command = 0x41;
   readLength = 3;

   writeLength = sprintf_s(buffer, "\x23%c%c%s%s\x0D", ADC2_OFFSET, command, registerNumber, registerValue);
   status = serialWrite((PUCHAR) buffer, &writeLength);
   QString writeLocation("writeADC2Register (serialWrite)");
   writeLocation.append(buffer);
   analyseErrorStatus(status, writeLocation);
   //emit writeMessage("Bytes written = " + QString::number(writeLength));
   status = serialRead((PUCHAR) buffer, &readLength);
   QString readLocation("writeADC2Register (serialRead)");
   writeLocation.append(buffer);
   analyseErrorStatus(status, readLocation);

   return status;
}

LONG AspectDetector::calcDACChannelValue(float voltage, QString *value, int channelNumber)
{
   LONG status = -1;
   float fvalue;
   char buffer[5];

   fvalue = voltage * 4095.0 / iniFile->getFloat("Controls/DAC Reference Voltage");
   sprintf_s(buffer, "%04X", int(fvalue + 0.5));

   value->replace(channelNumber * 4, 4, buffer);
   //emit writeMessage("STRING " + *value);

   status = 0;

   return status;
}

// TO DO: change int value to unsigned char.

LONG AspectDetector::readRegister(CHAR offset, int *value)
{
   LONG status;
   CHAR buffer[20];
   ULONG length;

   length = sprintf_s(buffer, "\x23%c\x41\x0D", offset);
   status = serialWrite((PUCHAR) buffer, &length);
   //emit writeMessage("Bytes written = " + QString::number(length));
   QString writeLocation("readRegister (serialWrite)");
   writeLocation.append(buffer);
   analyseErrorStatus(status, writeLocation);
   length = 5;
   status = serialRead((PUCHAR) buffer, &length);
   QString readLocation("readRegister (serialRead)");
   readLocation.append(buffer);
   analyseErrorStatus(status, readLocation);
   if (length == 5)
   {
      // Extract the register value from the return string
      sscanf_s(buffer, "*0%02X", value);
   }
   else
   {
      status = -1;
   }

   return status;
}

/*
 * Reads a number of bytes from the Aspect detector using the camlink serial interface.
 *
 * Can be invoked when an asynchronous GetImages is active.
 */

LONG AspectDetector::serialRead(PUCHAR buffer,
                                PULONG bufferLen)
{
   LONG status = 0;

   status = SerialRead(detectorHandle, // Handle to the frame grabber session
                       idx,            // Reserved set always to 0
                       buffer,         // Pointer to read buffer
                       bufferLen,      // Bytes to read then bytes read
                       timeout);       // Timeout (ms)
   buffer[*bufferLen] = NULL;

   return status;
}

/*
 * Sends a number of bytes to the Aspect detector using the camlink serial interface.
 *
 * Can be invoked when an asynchronous GetImages is active.
 */

LONG AspectDetector::serialWrite(CONST PUCHAR buffer,
                                 PULONG bufferLen)
{
   LONG status = 0;

   status = SerialWrite(detectorHandle, // Handle to the frame grabber session
                        idx,            // Reserved set always to 0
                        buffer,         // Pointer to write buffer
                        bufferLen,      // Bytes to send then bytes sent
                        timeout);       // Timeout (ms)

   return status;
}

int AspectDetector::asynchronousGetResult()
{
   LONG asError = -1;
   LONG niError = -1;
   LONG sysError = -1;
   char asErrorMsg[256];
   char niErrorMsg[256];
   char sysErrorMsg[256];
   int callStatus;
   int status = -1;

   /* Timeout currently set to a fixed value in the constructor.
    * Really a serial use timeout and may not be appropriate here.
    * When this method gets used need to check properly what timeout should be. */

   callStatus = GetResult(detectorHandle,
                          &asError,
                          asErrorMsg,
                          &niError,
                          niErrorMsg,
                          &sysError,
                          sysErrorMsg,
                          256,
                          timeout);

   if (callStatus != 0)
   {
      emit writeError("Call to GetResult failed.");
   }
   else if (asError != 0)
   {
      emit writeError("GetImages asError " + QString::fromUtf8(asErrorMsg));
   }
   else if (niError != 0)
   {
      emit writeError("GetImages niError " + QString::fromUtf8(niErrorMsg));
   }
   else if (sysError)
   {
      emit writeError("GetImages sysError " + QString::fromUtf8(sysErrorMsg));
   }
   else
   {
      status = 0;
   }

   return status;
}

/**********
 * Slots. *
 **********/

void AspectDetector::handleShowImage()
{
   //qDebug() << "SLOT: showImage event received at " << QTime::currentTime() << " thread id " << QThread::currentThreadId();
   imageDestToPixmap();
}

void AspectDetector::handleTriggeredReducedData()
{
   int status;

   emit externalTriggerReceived();
   status = triggerCollection();
}

void AspectDetector::handleTriggered()
{
   int status;

   if (mode == CONTINUOUS)
   {
      emit writeMessage("Starting Reduced Data State Machine." );
      startReducedDataStateMachine();
      //status = asynchronousGetResult();
      firstRunAfterInit = false;
   }
}

void AspectDetector::handleStop()
{
   updateState(READY);
   emit writeMessage("aSpect DAQ finished.");
}

void AspectDetector::configure(int mode, int quadrant)
{
   int status;

   setQuadrant(quadrant);
   if ((status = configure(mode)) == 0)
   {
      emit writeMessage("Aspect detector configured succssfully.");
   }
   else
   {
      emit writeError("Aspect detector configure failed " + QString::number(mode));
   }
}

void AspectDetector::registerCallback(p_newDataFileFunction fileCallback)
{
   LONG status = 0;
   //qDebug() << "Registering callback" << detectorHandle;
   //status = RegisterNewDataFileCallBack(detectorHandle, (p_newDataFileFunction) &fileCallback);
   status = RegisterNewDataFileCallBack(detectorHandle, fileCallback);
   //status = UnRegisterNewDataFileCallBack(detectorHandle);
   //qDebug() << "Callback registered, status = " << status;
}

void AspectDetector::unRegisterCallback()
{
   LONG status = 0;
   //() << "Un-Registering callback" << detectorHandle;
   status = UnRegisterNewDataFileCallBack(detectorHandle);
   //qDebug() << "Callback un-registered, status = " << status;
}

int AspectDetector::getTimeError()
{
   return timeError;
}

void AspectDetector::handleExecuteCommand(AspectDetector::DetectorCommand command, int ival1, int ival2)
{
   int status;

   //qDebug() << "Received command " << command << " thread id " << QThread::currentThreadId();
   //qDebug() << "Received command" << command << ival1 << ival2 << mode;

   if (command == COLLECT)
   {
      getImages(ival1, ival2);
   }
   else if (command == COLLECT_OFFSETS)
   {
      getOffsets();
   }
   else if (command == TRIGGER)
   {
      triggerCollection();
   }
   else if (command == CONNECT)
   {
      connectUp();
   }
   else if (command == CONFIGURE)
   {
      if (ival1 != mode || ival2 != quadrant)
      {
         configure(ival1, ival2);
      }
      else
      {
         // Already configured for the requested mode and quadrant.
         emit notifyMode(mode);
      }
   }
   else if (command == RECONFIGURE)
   {
      configure(ival1, ival2);
   }
   else if (command == INITIALISE)
   {
      if ((status = initialiseConnection()) == 0)
      {
         emit writeMessage("Detector connection initialised Ok");
      }
      else
      {
         emit writeError("Failed to initialise detector connection, status = " + QString::number(status));
      }
   }
   else if (command == ABORT)
   {
      abort(true);
   }
   else if (command == CLOSE)
   {
      if ((status = terminateConnection()) == 0)
      {
         emit writeMessage("Detector connection terminated Ok");
      }
      else
      {
         emit writeError("Failed to terminate detector connection, status = " + QString::number(status));
      }
   }
   else if (command == KILL)
   {
      abort(false);
      terminateConnection();
      aspectDetectorThread->exit();
      aspectDetectorThread->terminate();
   }
   else if (command == STATE)
   {
      updateState((DetectorState) ival1);
   }
   else if (command == STOP_TRIGGER)
   {
      abort(false);
      updateState(TRIGGERING_STOPPED);
   }
}

void AspectDetector::timerEvent(QTimerEvent *event)
{
   if (event->timerId() == getImagesTimerEventId)
   {
      timeError = (QDateTime::currentMSecsSinceEpoch() - startTime) - dataAcquisitionDuration;
      abort(true);
      QObject::killTimer(event->timerId());
   }
}

void AspectDetector::offsetsDialogAccepted()
{
   collectOffsets();
}
