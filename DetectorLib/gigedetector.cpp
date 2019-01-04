#include <QDebug>
#include <QThread>
#include <stdio.h>

/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX
#define STR_LENGTH 1024

#include <QImage>
#include <QDir>

#include "windowsevent.h"
#include "gigedetector.h"
#include "detectorfactory.h"

static PUCHAR bufferReady;
static PUCHAR returnBuffer;
static ULONG validFrames;
static ULONGLONG remainingFrames;
static double invalidTemperatureValue = -50.0;

HexitecOperationMode operationMode = {AS_CONTROL_DISABLED, AS_CONTROL_DISABLED,
                                      AS_CONTROL_DISABLED, AS_CONTROL_ENABLED,
                                      AS_CONTROL_DISABLED, AS_CONTROL_DISABLED,
                                      AS_CONTROL_DISABLED, AS_CONTROL_DISABLED,
                                      AS_CONTROL_DISABLED, AS_CONTROL_DISABLED,
                                      AS_CONTROL_DISABLED, 0 };
HexitecSystemConfig	systemConfig = {2, 10, AS_HEXITEC_ADC_SAMPLE_FALLING_EDGE, 4};

#ifdef DETECTORLIB_EXPORTS
static void __cdecl bufferCallBack(PUCHAR transferBuffer, ULONG frameCount)
{
   bufferReady = transferBuffer;
   validFrames = frameCount;
   remainingFrames -=validFrames;

   WindowsEvent *bufferReadyEvent = DetectorFactory::instance()->getBufferReadyEvent();
   WindowsEvent *showImageEvent = DetectorFactory::instance()->getShowImageEvent();

   bufferReadyEvent->SetEvent1();
   showImageEvent->SetEvent1();
}
#else
static void __cdecl bufferCallBack(PUCHAR transferBuffer, ULONG frameCount)
{
}
#endif

GigEDetector::GigEDetector(string aspectFilename)
{
   this->aspectFilename = QString::fromStdString(aspectFilename);
   constructorInit();
}

GigEDetector::GigEDetector(QString aspectFilename, const QObject *parent)
{
   this->aspectFilename = aspectFilename;
   constructorInit(parent);
}

void GigEDetector::constructorInit(const QObject *parent)
{
   timeout = 1000;
   collectDcTime = 0;
   gigEDetectorThread = new QThread();
   gigEDetectorThread->start();
   moveToThread(gigEDetectorThread);

   framesPerBuffer = 500;
   directory = "C:";
   prefix = "GigE_";
   xRes = 0;
   yRes = 0;
   /// 20180625 - Initialising triggeringMode because of above
   ///  xRes, yRes defaults no good; They cause detector error
   triggeringMode = Triggering::NO_TRIGGERING;
   vCal = 0.5;
   uMid = 1.0;
   detCtrl = 0;
   targetTemperature = 24.0;
   hvSetPoint = 0;
   appendTimestamp = false;
//   saveRaw = true;
   triggeringAvailable = false;
   triggerTimeout = 60000.0;
   readIniFile(this->aspectFilename);

   qRegisterMetaType<GigEDetector::DetectorCommand>("GigE::DetectorCommand");
   qRegisterMetaType<GigEDetector::DetectorState>("GigE::DetectorState");
   qRegisterMetaType<HANDLE>("HANDLE");


//   notifyStateEvent = new WindowsEvent(HEXITEC_NOTIFY_STATE, true);
//   notifyStateEvent = CreateEvent(NULL, FALSE, FALSE, HEXITEC_NOTIFY_STATE);

   connectUp(parent);
}

GigEDetector::~GigEDetector()
{
//   if (connected)
//   {
      terminateConnection();
//      connected = false;
//      free(imageInfoPtr);
//   }
   QThread::currentThread()->exit();
}

void GigEDetector::connectUp(const QObject *parent)
{

#ifdef DETECTORLIB_EXPORTS
   bufferReadyEvent = new WindowsEvent(HEXITEC_BUFFER_READY, true);
   bufferReadyEvent->connect1(parent, SLOT(handleBufferReady()));
   returnBufferReadyEvent = new WindowsEvent(HEXITEC_RETURN_BUFFER_READY, true);

   showImageEvent = new WindowsEvent(HEXITEC_SHOW_IMAGE, true);
   showImageEvent->connect1(parent, SLOT(handleShowImage()));

   connect(this , SIGNAL(executeReturnBufferReady(unsigned char *)), this, SLOT(handleReturnBufferReady()));
#endif
   connect(this, SIGNAL(executeGetImages()), this, SLOT(handleExecuteGetImages()));
}

void GigEDetector::handleBufferReady()
{
   updateState(COLLECTING);
   emit executeBufferReady(bufferReady, validFrames);
}

PUCHAR GigEDetector::getBufferReady()
{
   return bufferReady;
}

ULONG GigEDetector::getValidFrames()
{
   return validFrames;
}

void GigEDetector::handleReturnBufferReady()
{
   ReturnBuffer(detectorHandle, returnBuffer);
}

void GigEDetector::handleReturnBufferReady(unsigned char *returnBuffer, unsigned long validFrames)
{
    /// This was already commented out by dSoft (hence saveRaw being redundant)
   /*
   if ((saveRaw) && (mode == CONTINUOUS))
   {
      outFile.open(pathString, std::ofstream::binary | std::ofstream::app);
      outFile.write((const char *)returnBuffer, validFrames * frameSize);
      outFile.close();
   }
*/
   ReturnBuffer(detectorHandle, returnBuffer);
}

void GigEDetector::setTargetTemperature(double targetTemperature)
{
   handleSetTargetTemperature(targetTemperature);
}

void GigEDetector::setTriggeringMode(int triggeringMode)
{
   this->triggeringMode = (Triggering)triggeringMode;
}

void GigEDetector::setTtlInput(int ttlInput)
{
   this->ttlInput = (TtlInput)ttlInput;
}

int GigEDetector::getTtlInput()
{
   return ttlInput;
}

void GigEDetector::handleSetTargetTemperature(double targetTemperature)
{
   int status = -1;

   this->targetTemperature = targetTemperature;

   try
   {
      status = SetDAC(detectorHandle, &vCal, &uMid, &hvSetPoint, &detCtrl, &targetTemperature, timeout);
      showError("SetDAC", status);
   }
   catch (DetectorException &ex)
   {
      emit writeError(ex.getMessage());
      qDebug() << Q_FUNC_INFO << "caught: " << ex.getMessage();
   }
}

void GigEDetector::setHV(double *voltage)
{
   handleSetHV(*voltage);
}

void GigEDetector::handleSetHV(double voltage)
{
   int status = -1;

   this->hvSetPoint = voltage;

   try
   {
      status = SetDAC(detectorHandle, &vCal, &uMid, &hvSetPoint, &detCtrl, &targetTemperature, timeout);
      showError("SetDAC", status);
   }
   catch (DetectorException &ex)
   {
      emit writeError(ex.getMessage());
      qDebug() << Q_FUNC_INFO << "caught: " << ex.getMessage();
   }
}

void GigEDetector::handleAppendTimestamp(bool appendTimestamp)
{
    this->appendTimestamp = appendTimestamp;
}

//void GigEDetector::setSaveRaw(bool *saveRaw)
//{
//   this->saveRaw = *saveRaw;
//}

unsigned long long GigEDetector::getRemainingFrames()
{
   return remainingFrames;
}

//void GigEDetector::handleSaveRawChanged(bool saveRaw)
//{
//   this->saveRaw = saveRaw;
//}

void GigEDetector::handleSetTriggerTimeout(double triggerTimeout)
{
   this->triggerTimeout = triggerTimeout * 1000.0;
}

int GigEDetector::initialiseConnection()
{
   int status = -1;
   try
   {
      status = initialise();
      RegisterTransferBufferReadyCallBack(detectorHandle, bufferCallBack);
   }
   catch (DetectorException &ex)
   {
      emit writeError(ex.getMessage());
      qDebug() << Q_FUNC_INFO << "caught: " << ex.getMessage();
   }
   return status;
}

int GigEDetector::initialiseConnection(p_bufferCallBack bufferCallBack)
{
   int status = -1;
   try
   {
      status = initialise();
      RegisterTransferBufferReadyCallBack(detectorHandle, bufferCallBack);
   }
   catch (DetectorException &ex)
   {
      emit writeError(ex.getMessage());
      qDebug() << Q_FUNC_INFO << "caught: " << ex.getMessage();
   }
   return status;
}

int GigEDetector::initialise(Triggering triggering)
{
   LONG status = -1;
   CONST LPSTR deviceDescriptor = (const LPSTR )"";
   ULONG pleoraErrorCodeStrLen = STR_LENGTH;
   ULONG pleoraErrorDescriptionLen = STR_LENGTH;
   ULONG pleoraErrorCode = -1;
   CHAR pleoraErrorCodeStr[STR_LENGTH] = {0};
   CHAR pleoraErrorDescription[STR_LENGTH] = {0};
   GigEDeviceInfoStr deviceInfo = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
   UCHAR customerId = 0x01;
   UCHAR projectId = 0x12;
   UCHAR version = 0x02;
   UCHAR forceEqualVersion = 0x0;
//   ULONG timeOut = 1000;

   updateState(INITIALISING);

   status = InitDevice(&detectorHandle, deviceDescriptor, &pleoraErrorCode, pleoraErrorCodeStr,
                       &pleoraErrorCodeStrLen, pleoraErrorDescription, &pleoraErrorDescriptionLen);
   showError("InitDevice", status);    /// showError() caught by either of the two ::initialseConnection() funcs

   status = GetDeviceInformation(detectorHandle, &deviceInfo);
   showError("GetDeviceInformation", status);
   frameTime = 0;


   status = CloseSerialPort(detectorHandle);
   status = ClosePipeline(detectorHandle);
   status = CloseStream(detectorHandle);
   status = OpenSerialPort(detectorHandle, 2, 2048, 1, 0x0d );
   showError("OpenSerialPort", status);

   status = OpenStream(detectorHandle);
   showError("OpenStream", status);

   status = CheckFirmware(detectorHandle,&customerId, &projectId, &version, forceEqualVersion, timeout);
   showError("CheckFirmware", status);
   if (!status)
   {
      qDebug() <<"Detector in use - customer ID:" << customerId;
      qDebug() <<"Detector in use - project ID:" << projectId;
      qDebug() <<"Detector in use - firmware version:" << version;
      triggeringAvailable = true;
   }
   else
   {
      triggeringAvailable = false;
   }

   status = configureDetector();
   if (!status)
   {
      qDebug() << "Configure Detector - frames per buffer:" << framesPerBuffer;
      qDebug() << "Configure Detector - frameTime:" << frameTime ;
      qDebug() << "Configure Detector - collectDcTime:" << collectDcTime ;
      qDebug() << "Configure Detector - resolution:" << xRes << "x" << yRes;
   }


   status = SetDAC(detectorHandle, &vCal, &uMid, &hvSetPoint, &detCtrl, &targetTemperature, timeout);
   showError("SetDAC", status);
#ifdef DETECTORLIB_EXPORTS
   RegisterTransferBufferReadyCallBack(detectorHandle, bufferCallBack);
#endif
   updateState(READY);
   QString message = QString("Detector connection to IP ");
   message.append(deviceInfo.IpAddress);
   message.append (" initialised Ok");
   emit writeMessage(message);
   emit triggeringAvailableChanged(triggeringAvailable);
//   emit detectorResolutionSet(xRes * sensorColumns, yRes * sensorRows);

   return status;
}

int GigEDetector::configure(bool triggeringSuspended)
{
   LONG status = -1;

   try
   {
      updateState(INITIALISING);

      status = CloseSerialPort(detectorHandle);
      status = ClosePipeline(detectorHandle);
      status = CloseStream(detectorHandle);

      status = OpenSerialPort(detectorHandle, 2, 2048, 1, 0x0d);
      showError("OpenSerialPort", status);

      status = OpenStream(detectorHandle);
      showError("OpenStream", status);

      status = configureDetector(triggeringSuspended);

      updateState(READY);
   }
   catch (DetectorException &ex)
   {
      emit writeError(ex.getMessage());
      qDebug() << Q_FUNC_INFO << "caught: " << ex.getMessage();
   }

   return status;
}

int GigEDetector::configureDetector(bool triggeringSuspended)
{
   LONG status = -1;
   ULONG timeOut = 1000;

   if (triggeringSuspended)
   {
      triggerConfigMode = Triggering::NO_TRIGGERING;
   }
   else
   {
      triggerConfigMode = triggeringMode;
   }
   qDebug() << "triggerConfigMode" << triggerConfigMode;
   switch (triggerConfigMode)
   {
      case Triggering::NO_TRIGGERING:
         qDebug() << "CONFIGURE: No Triggering";
         status = ConfigureDetectorWithTrigger(detectorHandle, &sensorConfig, &operationMode, &systemConfig,
                                    &xRes, &yRes, &frameTime, &collectDcTime, timeOut, AS_CONTROL_DISABLED, AS_CONTROL_DISABLED);
         showError( "ConfigureDetector", status);  /// showError() caught by ::configure()
         break;
      case Triggering::STANDARD_TRIGGERING:
         qDebug() << "CONFIGURE: Standard Triggering";
         status = ConfigureDetectorWithTrigger(detectorHandle, &sensorConfig, &operationMode, &systemConfig,
                                               &xRes, &yRes, &frameTime, &collectDcTime, timeOut, AS_CONTROL_DISABLED, AS_CONTROL_ENABLED);
         showError("ConfigureDetectorWithTrigger", status);
         break;
      case Triggering::SYNCHRONISED_TRIGGERING:
         qDebug() << "CONFIGURE: Synchronised Triggering";
         status = ConfigureDetectorWithTrigger(detectorHandle, &sensorConfig, &operationMode, &systemConfig,
                                               &xRes, &yRes, &frameTime, &collectDcTime, timeOut, AS_CONTROL_ENABLED, AS_CONTROL_ENABLED);
         showError("ConfigureDetectorWithTrigger", status);
         break;
      default:
         triggeringMode = Triggering::INVALID_TRIGGERING;
         break;
   }

   status = setImageFormat(xRes, yRes);
   status = CreatePipeline(detectorHandle, 512, 100, framesPerBuffer);
   showError( "CreatePipeline", status);

   return status;
}

int GigEDetector::terminateConnection()
{
   LONG status = -1;

   try
   {
      status = CloseSerialPort(detectorHandle);
      showError( "CloseSerialPort", status);

      status = ClosePipeline(detectorHandle);
      showError( "ClosePipeline", status);

      status = CloseStream(detectorHandle);
      showError( "CloseStream", status);

      status = ExitDevice(detectorHandle);
      showError( "ExitDevice", status);
   }
   catch (DetectorException &ex)
   {
      emit writeError(ex.getMessage());
      qDebug() << Q_FUNC_INFO << "caught: " << ex.getMessage();
   }
   updateState(IDLE);

   return status;
}

int GigEDetector::getDetectorValues(double *rh, double *th, double *tasic, double *tdac, double *t, double *hvCurrent)
{
    int status = -1;
    double v3_3, hvMon, hvOut, v1_2, v1_8, v3, v2_5, v3_31n, v1_651n, v1_8ana, v3_8ana, peltierCurrent, ntcTemperature;
    UCHAR t1, t2, t3;

    status = ReadEnvironmentValues(detectorHandle, rh, th, tasic, tdac, t, timeout);
    showError("ReadEnvironmentValues", status);    /// Caught by DetectorMonitor's calling funcs

    status = ReadOperatingValues(detectorHandle, &v3_3, &hvMon, &hvOut, &v1_2, &v1_8, &v3, &v2_5, &v3_31n, &v1_651n, &v1_8ana, &v3_8ana, &peltierCurrent, &ntcTemperature, timeout);
    showError("ReadOperatingValues", status);

    status = GetTriggerState(detectorHandle, &t1, &t2, &t3, timeout);
    showError("GetTriggerState", status);

    if (*t < invalidTemperatureValue)
    {
       status = ReadEnvironmentValues(detectorHandle, rh, th, tasic, tdac, t, timeout);
       showError("ReadEnvironmentValues", status);
    }

    *hvCurrent = hvOut;
/*
    *rh = 20.0;
    *th = 25.0;
    *tasic = 26.0;
    *tdac = 27.0;
    *t = 28.0;
*/
    return status;
}

int GigEDetector::setImageFormat(unsigned long xResolution, unsigned long yResolution)
{
   LONG status = -1;

   // Fix to satisfy ANSI string handling
   const LPSTR pArg2 = LPSTR("Mono14");
   const LPSTR pArg7 = LPSTR("One");
   const LPSTR pArg8 = LPSTR("Off");

   // Unmodified:
   //status = SetFrameFormatControl(detectorHandle, "Mono14", xResolution, yResolution, 0, 0, "One", "Off");
   status = SetFrameFormatControl(detectorHandle, pArg2, xResolution, yResolution, 0, 0, pArg7, pArg8);
   showError( "SetFrameFormatControl", status);    /// showError() catch by ::configure(), ::initialise() respectively
   frameSize = xResolution * xResolution * 2;

   return status;
}

void GigEDetector::setCommand(DetectorCommand command)
{
   this->command = command;
}

void GigEDetector::collectImage()
{
   setMode(CONTINUOUS);
   getImages(0, 0);
}

// TODO: Why not use switch statement?
void GigEDetector::handleExecuteCommand(GigEDetector::DetectorCommand command, int ival1, int ival2)
{
   int status;

   if (command == INITIALISE)
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
   if (command == CONFIGURE)
   {
      if ((status = configure(ival1)) == 0)
      {
         emit writeMessage("Detector configured Ok");
      }
      else
      {
         emit writeError("Failed to configure detector, status = " + QString::number(status));
      }
   }
   if (command == RESTART)
   {
      restartImages(ival1);
   }

   if (command == COLLECT)
   {
      getImages(ival1, ival2);
   }
   else if (command == COLLECT_OFFSETS)
   {
        // TODO: 20180 618, Not called anywhere
   }

   else if (command == ABORT)
   {
      abort(false);
//      abort(true);
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
      gigEDetectorThread->exit();
      gigEDetectorThread->terminate();
   }
   else if (command == STATE)
   {
      updateState((DetectorState) ival1);
   }
}

void GigEDetector::getImages(int count, int ndaq)
{
   this->count = count;

   if (mode == CONTINUOUS && appendTimestamp)
   {
     setGetImageParams();
   }

   if (ndaq == 0 && offsetsOn)
   {
      updateState(OFFSETS_PREP);
      emit prepareForOffsets();
   }
   else
   {
      if ( !QDir(directory).exists())
      {
         QDir().mkpath(directory);
      }
      handleReducedDataCollection();
   }
}

void GigEDetector::setGetImageParams()
{
   QString path;

   path = directory + "/" + prefix;

   if (timestampOn)
   {
      path += "_";
      path += QDateTime::currentDateTime().toString("yyMMdd_hhmmss");
   }

   path.replace(QString("/"), QString("\\"));
   sprintf_s(processingFilename, "%s", path.toUtf8().data());
   emit imageStarted(processingFilename);
   sprintf_s(pathString, "%s.bin", processingFilename);

   if ( !QDir(directory).exists())
   {
      QDir().mkpath(directory);
   }

   imgCntAverage = 1;
}

WindowsEvent *GigEDetector::getBufferReadyEvent()
{
   return bufferReadyEvent;
}

HANDLE *GigEDetector::getTransferBufferReadyEvent()
{
   return &transferBufferReadyEvent;
}

HANDLE *GigEDetector::getReturnBufferReadyEvent()
{
   return &returnBufferReadyEvent;
}

WindowsEvent *GigEDetector::getShowImageEvent()
{
   return showImageEvent;
}

HANDLE *GigEDetector::getNotifyStateEvent()
{
   return &notifyStateEvent;
}

void GigEDetector::handleReducedDataCollection()
{
   emit executeGetImages();
}

void GigEDetector::restartImages(bool startOfImage)
{
//   qDebug() << "GigEDetector::restartImages() called, startOfImage = " << startOfImage;
   if (mode == CONTINUOUS && appendTimestamp)
   {
     setGetImageParams();
   }

   if (startOfImage && offsetsOn)
   {
      updateState(OFFSETS_PREP);
      emit prepareForOffsets();
   }

   handleExecuteGetImages(startOfImage);
//   emit executeAcquireImages(startOfImage);
}

void GigEDetector::handleExecuteGetImages(bool startOfImage)
{
   if ((triggerConfigMode == STANDARD_TRIGGERING) || (triggerConfigMode == SYNCHRONISED_TRIGGERING))
   {
      updateState(WAITING_TRIGGER);
   }
   else
   {
      updateState(COLLECTING);
   }
   emit executeAcquireImages(startOfImage);
}

void GigEDetector::handleExecuteOffsets()
{
   if (offsetsOn)
   {
      updateState(OFFSETS);
//      collectOffsets();
      collectOffsetValues();
      updateState(COLLECTING_PREP);
      emit prepareForDataCollection();
   }
   else
   {
      updateState(READY);
   }
}

void GigEDetector::collectOffsets()
{
   updateState(OFFSETS);
   collectOffsetValues();
   updateState(READY);

}

LONG GigEDetector::collectOffsetValues()
{
   int status = -1;
   try
   {
      status = CollectOffsetValues(detectorHandle, 1000, collectDcTime);								// make sure to have stable operating conditions (high voltage, temperature, x-ray turned off)
      showError("CollectOffsetValues", status);
   }
   catch (DetectorException &ex)
   {
      emit writeError(ex.getMessage());
      qDebug() << Q_FUNC_INFO << "caught: " << ex.getMessage();
   }

   return 0;
}

void GigEDetector::acquireImages(bool startOfImage)
{
   int status = -1;
   ULONGLONG framesAcquired;
   double durationSeconds = dataAcquisitionDuration/1000.0;
   ULONG frameCount = (durationSeconds/frameTime) + 0.5;
   ULONG frameTimeout = (ULONG)(frameTime * 2500.0);

   if (startOfImage)
   {
      qDebug() <<"Starting a new collection, frameCount = " << frameCount;
      remainingFrames = frameCount;
      totalFramesAcquired = 0;
   }
   else
   {
      qDebug() <<"Restart collection, remainingFrames = " << remainingFrames;
      frameCount = remainingFrames;
   }

   if( frameTimeout < 100 )
   {
      frameTimeout = 100;
   }

   if (triggerConfigMode == NO_TRIGGERING)
   {
      /// 20180627: Put these 3 lines within try, catch
      ///   otherwise any error will crash showError() call
      try
      {
         status = SetFrameTimeOut(detectorHandle, frameTimeout);
         status = AcquireFrames(detectorHandle, frameCount, &framesAcquired, frameTimeout);
         showError("AcquireFrames", status);
      }
      catch (DetectorException &ex)
      {
         QString message = "No Triggering collection terminated: " + ex.getMessage();
         qDebug() << "No Triggering collection aborted";
         emit writeError(message);
         remainingFrames = 0;
         emit cancelDataCollection();
      }
      /// End of added try, catch block
///SHOULD THIS BE ADDED???
      if (mode == CONTINUOUS)
      {
//         qDebug() << "IN SHOULD THIS BE ADDED??? section!!!";
         //emit imageComplete(framesAcquired);
      }
   }
   else
   {
      try
      {
         status = SetTriggeredFrameCount(detectorHandle, frameCount, frameTimeout);
         showError("SetTriggeredFrameCount", status);

         status = SetFrameTimeOut(detectorHandle, frameTimeout);
         status = AcquireFrames(detectorHandle, frameCount, &framesAcquired, triggerTimeout);
         showError("AcquireFrames Triggered", status);
      }
      catch (DetectorException &ex)
      {
         QString message = "Triggered collection terminated: " + ex.getMessage();
         qDebug() << "Triggered collection aborted - NO TRIGGER";
         emit writeError(message);
         remainingFrames = 0;
         emit cancelDataCollection();
      }
      qDebug() << "Collect triggered image over!";
   }

   totalFramesAcquired += framesAcquired;

   if (mode == CONTINUOUS && remainingFrames == 0)
   {
      emit imageComplete(totalFramesAcquired);
   }
   updateState(READY);
}

int GigEDetector::getLoggingInterval()
{
   return loggingInterval;
}

void GigEDetector::beginMonitoring()
{
   emit enableMonitoring();
}

bool GigEDetector::getTriggeringAvailable()
{
   return triggeringAvailable;
}

void GigEDetector::handleStop()
{
   updateState(READY);
   emit writeMessage("GigE DAQ finished.");
}

void GigEDetector::abort(bool restart)
{
   if (restart)
   {
      StopAcquisition(detectorHandle);
      qDebug() << "Acquisition STOPPED - WITH restart";
   }
   else
   {
      StopAcquisition(detectorHandle);
      qDebug() << "Acquisition STOPPED - NO restart";
      updateState(READY);
   }
}

void GigEDetector::updateState(DetectorState state)
{
   this->state = state;
   emit notifyState(state);
//   SetEvent(notifyStateEvent);
}

GigEDetector::DetectorState GigEDetector::getState()
{
   return state;
}

void GigEDetector::setMode(Mode mode)
{
   this->mode = mode;
}

ULONGLONG GigEDetector::setDataAcquisitionDuration(double *dataAcquisitionDuration)
{
   double durationSeconds = *dataAcquisitionDuration/1000.0;
   remainingFrames = (durationSeconds/frameTime) + 0.5;

   this->dataAcquisitionDuration = *dataAcquisitionDuration;

   return remainingFrames;
}

ULONGLONG GigEDetector::setDataAcquisitionDuration(double dataAcquisitionDuration)
{
   double durationSeconds = dataAcquisitionDuration/1000.0;
   remainingFrames = (durationSeconds/frameTime) + 0.5;

   this->dataAcquisitionDuration = dataAcquisitionDuration;

   return remainingFrames;
}

void GigEDetector::setDataDirectory(string  *directory)
{
   this->directory = QString::fromStdString(*directory);
}

void GigEDetector::setDirectory(QString directory)
{
   this->directory = directory;
}

string GigEDetector::getDataDirectory()
{
   return this->directory.toStdString();
}

QString GigEDetector::getDirectory()
{
   return this->directory;
}

void GigEDetector::setDataPrefix(string *prefix)
{
   this->prefix = QString::fromStdString(*prefix);
}

void GigEDetector::setPrefix(QString prefix)
{
   this->prefix = prefix;
}

string GigEDetector::getDataPrefix()
{
   return prefix.toStdString();
}

QString GigEDetector::getPrefix()
{
   return prefix;
}

void GigEDetector::setTimestampOn(bool timestampOn)
{
   this->timestampOn = timestampOn;
}

bool GigEDetector::getTimestampOn()
{
   return timestampOn;
}

GigEDetector::Mode GigEDetector::getMode()
{
   return mode;
}

void GigEDetector::enableDarks()
{
   offsetsOn = true;
}

void GigEDetector::disableDarks()
{
   offsetsOn = false;
}

void GigEDetector::imageDestToPixmap()
{
   QVector<QRgb> colorTable;

   for (int i = 0; i < 256; i++)
   {
      colorTable.push_back(QColor(i, i, i).rgb());
   }

   if (getBufferReady() != NULL)
   {
      charImageDest = getImage(0); // TO DO : Free the memory malloc'd
      QImage image(charImageDest, xRes, yRes, QImage::Format_Indexed8);

      image.setColorTable(colorTable);

      imagePixmap = QPixmap::fromImage(image);
      free(imageDest);
      emit imageAcquired(imagePixmap);

   }
}

unsigned char *GigEDetector::getImage(int imageNumber)
{
   int imageSize = xRes * yRes;
   int segmentSize = xRes * yRes * 2;
   unsigned char *buffer;
   unsigned char *image;
   short current, min = 32767, max = -32768, range;


   buffer = getBufferReady();
   imageDest = (short *) malloc(imageSize * sizeof(short)); // free()'d in ::imageDEstToPixmap
   image = (unsigned char *) malloc(imageSize * sizeof(unsigned char)); // free() where? -> detCtrlForm, SLOT(setPixMap())
   memcpy(imageDest, buffer, segmentSize);
   returnBuffer = buffer;
   emit executeReturnBufferReady(returnBuffer);

   for (int i = 0; i < imageSize; i++)
   {
      current = imageDest[i];
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

   switch (range)
   {
   case 0:
      for (int i = 0; i < imageSize; i++)
      {
         image[i] = 0;
      }
      break;
   default:
      for (int i = 0; i < imageSize; i++)
      {
         image[i] = ((imageDest[i] - min) * 255) / range;
      }
      break;
   }

   return image;
}

void GigEDetector::handleShowImage()
{
   if (mode == GIGE_DEFAULT)
   {
      imageDestToPixmap();
   }
}

LONG GigEDetector::readIniFile(QString aspectFilename)
{
   LONG status = -1;
   int loggingInterval = 1;
   double vCal;

   status = 0;
   iniFile = new IniFile(aspectFilename);
   framesPerBuffer = iniFile->getInt("Control-Settings/Frames Per Buffer");

   if ((loggingInterval = iniFile->getInt("Control-Settings/Logging Interval")) != QVariant(INVALID))
   {
      this->loggingInterval = loggingInterval;
   }
   else
   {
      this->loggingInterval = 1;
   }

   if ((vCal = iniFile->getDouble("Control-Settings/VCAL")) != QVariant(INVALID))
   {
      this->vCal = vCal;
   }

   sensorConfig.Gain = (HexitecGain)iniFile->getInt("Control-Settings/Gain");
   sensorConfig.Row_S1 = iniFile->getInt("Control-Settings/Row -> S1");
   sensorConfig.S1_Sph = iniFile->getInt("Control-Settings/S1 -> Sph");
   sensorConfig.Sph_S2 = iniFile->getInt("Control-Settings/Sph -> S2");
   sensorConfig.Vcal2_Vcal1 = iniFile->getInt("Control-Settings/VCAL2 -> VCAL1");
   sensorConfig.WaitClockCol = 1;
   sensorConfig.WaitClockRow = 8;

   sensorConfig.SetupRow = initSetupRegister("Row");
   sensorConfig.SetupCol = initSetupRegister("Column");

   return status;
}

HexitecSetupRegister GigEDetector::initSetupRegister(QString type)
{
   HexitecSetupRegister setupRegister;
   QString iniFileKey = "Sensor-Config/";
   QString endPart;
   std::string configCharacters;

   if (type == "Column")
   {
      endPart = "Channel";
   }
   else
   {
      endPart = "Block";
   }

   iniFileKey += type;

   configCharacters = (iniFile->getString(iniFileKey + "En_1st" + endPart)).toStdString();
   configCharacters += (iniFile->getString(iniFileKey + "En_2nd" + endPart)).toStdString();
   configCharacters += (iniFile->getString(iniFileKey + "En_3rd" + endPart)).toStdString();
   configCharacters += (iniFile->getString(iniFileKey + "En_4th" + endPart)).toStdString();

   configCharacters2Bytes(configCharacters, setupRegister.ReadEn);

   configCharacters = (iniFile->getString(iniFileKey + "Pwr1st" + endPart)).toStdString();
   configCharacters += (iniFile->getString(iniFileKey + "Pwr2nd" + endPart)).toStdString();
   configCharacters += (iniFile->getString(iniFileKey + "Pwr3rd" + endPart)).toStdString();
   configCharacters += (iniFile->getString(iniFileKey + "Pwr4th" + endPart)).toStdString();

   configCharacters2Bytes(configCharacters, setupRegister.PowerEn);

   configCharacters = (iniFile->getString(iniFileKey + "Cal1st" + endPart)).toStdString();
   configCharacters += (iniFile->getString(iniFileKey + "Cal2nd" + endPart)).toStdString();
   configCharacters += (iniFile->getString(iniFileKey + "Cal3rd" + endPart)).toStdString();
   configCharacters += (iniFile->getString(iniFileKey + "Cal4th" + endPart)).toStdString();

   configCharacters2Bytes(configCharacters, setupRegister.CalEn);

   return setupRegister;
}

void GigEDetector::configCharacters2Bytes(std::string configCharacters, unsigned char *result)
{
   unsigned char zero = '0';

   for (int i = 0; i < AS_HEXITEC_SETUP_REGISTER_SIZE; i++)
   {
      result[i] = 0;
      for (int j = 0; j < 8; j++)
      {
         result[i] = result[i] << 1;
         result[i] |= ((configCharacters[i * 8 + j]) - zero);
      }
   }
}


void GigEDetector::showError(const LPCSTR context, long asError)
{
   LONG result = 1;
   LONG sysError = 0;
   ULONG pleoraErrorCodeStrLen = STR_LENGTH;
   ULONG pleoraErrorDescriptionLen = STR_LENGTH;
   CHAR asErrorMessage[STR_LENGTH] = { 0 };
   CHAR sysErrorMessage[STR_LENGTH] = { 0 };
   ULONG pleoraErrorCode = 0;
   CHAR	pleoraErrorCodeStr[STR_LENGTH] = { 0 };
   CHAR	pleoraErrorDescription[STR_LENGTH] = { 0 };

   sysError = GetAsErrorMsg( asError, asErrorMessage, STR_LENGTH );

   if( sysError )
   {
      qDebug() << "System Error";
      sysError = GetSystemErrorMsg( sysError, sysErrorMessage, STR_LENGTH );
      qDebug() << "\n%s\n\tSystem Error" << context << sysError << "occured trying to get aSpect error message for aSpect error: " << sysErrorMessage << asError;
      return;
   }
   else if( asError )
   {
      qDebug() << "Aspect Error";
      if( context != "InitDevice" )
      {
         result = GetLastResult(detectorHandle, &pleoraErrorCode, pleoraErrorCodeStr, &pleoraErrorCodeStrLen, pleoraErrorDescription, &pleoraErrorDescriptionLen );
      }
      else
      {
         result = 0;
      }
   }

   if( !result )
   {
      qDebug() << "Pleora Error";

      errorMessage = "aSpect Result: ";

      errorMessage.append(QString::fromLatin1((char *)context));
      errorMessage.append(QString::fromLatin1((char *)asErrorMessage));

      errorMessage.append("Pleora Result Code: ");
      errorMessage.append(QString::number(pleoraErrorCode));
      errorMessage.append(", Pleora Result Code String: ");
      errorMessage.append(QString::fromLatin1((char *)pleoraErrorCodeStr));

      qDebug() << "aSpect Result: " << context << asErrorMessage;
      qDebug() << "Pleora Result Code:" << pleoraErrorCode;
      qDebug() << "Pleora Result Code String:" << pleoraErrorCodeStr;
      qDebug() << "Pleora Result Description:" << pleoraErrorDescription;

      throw DetectorException(errorMessage);
   }

}
