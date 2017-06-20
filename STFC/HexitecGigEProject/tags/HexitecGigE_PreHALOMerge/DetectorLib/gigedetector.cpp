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

HexitecOperationMode operationMode = {AS_CONTROL_DISABLED, AS_CONTROL_DISABLED,
                                      AS_CONTROL_DISABLED, AS_CONTROL_ENABLED,
                                      AS_CONTROL_DISABLED, AS_CONTROL_DISABLED,
                                      AS_CONTROL_DISABLED, AS_CONTROL_DISABLED,
                                      AS_CONTROL_DISABLED, AS_CONTROL_DISABLED,
                                      AS_CONTROL_DISABLED, 0 };
HexitecSystemConfig	systemConfig = {2, 10, AS_HEXITEC_ADC_SAMPLE_FALLING_EDGE, 4};

static void __cdecl bufferCallBack(PUCHAR transferBuffer, ULONG frameCount)
{
   bufferReady = transferBuffer;
   validFrames = frameCount;
   WindowsEvent *bufferReadyEvent = DetectorFactory::instance()->getBufferReadyEvent();
   WindowsEvent *showImageEvent = DetectorFactory::instance()->getShowImageEvent();

   bufferReadyEvent->SetEvent1();
   showImageEvent->SetEvent1();
}

GigEDetector::GigEDetector(QString aspectFilename, const QObject *parent)
{
   timeout = 1000;
   collectDcTime = 0;
   this->aspectFilename = aspectFilename;
   gigEDetectorThread = new QThread();
   gigEDetectorThread->start();
   moveToThread(gigEDetectorThread);

   framesPerBuffer = 500;
   directory = "C:";
   prefix = "GigE_";
   xRes = 80;
   yRes = 80;
   vCal = 0.5;
   uMid = 1.0;
   detCtrl = 0;
   targetTemperature = 20.0;
   hvSetPoint = 0;
   appendTimestamp = false;
   saveRaw = true;

   readIniFile(this->aspectFilename);

   qRegisterMetaType<GigEDetector::DetectorCommand>("GigE::DetectorCommand");
   qRegisterMetaType<GigEDetector::DetectorState>("GigE::DetectorState");
   qRegisterMetaType<HANDLE>("HANDLE");
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
   bufferReadyEvent = new WindowsEvent(HEXITEC_BUFFER_READY, true);
   bufferReadyEvent->connect1(parent, SLOT(handleBufferReady()));
   returnBufferReadyEvent = new WindowsEvent(HEXITEC_RETURN_BUFFER_READY, true);
   showImageEvent = new WindowsEvent(HEXITEC_SHOW_IMAGE, true);
   showImageEvent->connect1(parent, SLOT(handleShowImage()));
   connect(this , SIGNAL(executeReturnBufferReady(unsigned char *)), this, SLOT(handleReturnBufferReady()));
   connect(this, SIGNAL(executeGetImages()), this, SLOT(handleExecuteGetImages()));
}

void GigEDetector::handleBufferReady()
{
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
   if ((saveRaw) && (mode == CONTINUOUS))
   {
      outFile.open(pathString, std::ofstream::binary | std::ofstream::app);
      outFile.write((const char *)returnBuffer, validFrames * frameSize);
      outFile.close();
   }

   ReturnBuffer(detectorHandle, returnBuffer);
}

void GigEDetector::handleSetTargetTemperature(double targetTemperature)
{
    int status = -1;

    this->targetTemperature = targetTemperature;

    status = SetDAC(detectorHandle, &vCal, &uMid, &hvSetPoint, &detCtrl, &targetTemperature, timeout);
    showError("SetDAC", status);
}

void GigEDetector::handleSetHV(double voltage)
{
    int status = -1;

    this->hvSetPoint = voltage;

    status = SetDAC(detectorHandle, &vCal, &uMid, &hvSetPoint, &detCtrl, &targetTemperature, timeout);
    showError("SetDAC", status);
}

void GigEDetector::handleAppendTimestamp(bool appendTimestamp)
{
    this->appendTimestamp = appendTimestamp;
}

void GigEDetector::handleSaveRawChanged(bool saveRaw)
{
    qDebug() << "GigEDetector::handleSaveRawChanged with:" <<saveRaw;
    this->saveRaw = saveRaw;
}

int GigEDetector::initialiseConnection()
{
   LONG status = -1;
   CONST LPSTR deviceDescriptor = "";
   ULONG pleoraErrorCodeStrLen = STR_LENGTH;
   ULONG pleoraErrorDescriptionLen = STR_LENGTH;
   ULONG pleoraErrorCode = -1;
   CHAR pleoraErrorCodeStr[STR_LENGTH] = {0};
   CHAR pleoraErrorDescription[STR_LENGTH] = {0};
   GigEDeviceInfoStr deviceInfo = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
   updateState(INITIALISING);

   status = InitDevice(&detectorHandle, deviceDescriptor, &pleoraErrorCode, pleoraErrorCodeStr,
                       &pleoraErrorCodeStrLen, pleoraErrorDescription, &pleoraErrorDescriptionLen);
   showError("InitDevice", status);

   status = GetDeviceInformation(detectorHandle, &deviceInfo);
   showError("GetDeviceInformation", status);

   status = CloseSerialPort(detectorHandle);
   status = ClosePipeline(detectorHandle);
   status = CloseStream(detectorHandle);
   status = OpenSerialPort(detectorHandle, 2, 2048, 1, 0x0d );
   showError("OpenSerialPort", status);
   frameTime = 0;

   status = OpenStream(detectorHandle);
   showError("OpenStream", status);

   status = ConfigureDetector(detectorHandle, &sensorConfig, &operationMode, &systemConfig,
                              &xRes, &yRes, &frameTime, &collectDcTime, 1000);
   showError( "ConfigureDetector", status);
   if (!status)
   {
      qDebug() <<"Configure Detector - frames per buffer:" << framesPerBuffer;
      qDebug() << "Configure Detector - frameTime:" << frameTime ;
      qDebug() <<"Configure Detector - collectDcTime:" << collectDcTime ;
   }

   status = setImageFormat(xRes, yRes);
   status = CreatePipeline(detectorHandle, 512, 100, framesPerBuffer);
   showError( "ConfigureDetector", status);

   status = SetDAC(detectorHandle, &vCal, &uMid, &hvSetPoint, &detCtrl, &targetTemperature, timeout);
   showError("SetDAC", status);

   RegisterTransferBufferReadyCallBack(detectorHandle, bufferCallBack);
   updateState(READY);
   QString message = QString("Detector connection to IP ");
   message.append(deviceInfo.IpAddress);
   message.append (" initialised Ok");
   emit writeMessage(message);

   return status;
}

int GigEDetector::terminateConnection()
{
   LONG status = -1;

   status = CloseSerialPort(detectorHandle);
   showError( "CloseSerialPort", status);

   status = ClosePipeline(detectorHandle);
   showError( "ClosePipeline", status);

   status = CloseStream(detectorHandle);
   showError( "CloseStream", status);

   status = ExitDevice(detectorHandle);
   showError( "ExitDevice", status);

   updateState(IDLE);

   return status;
}

int GigEDetector::getDetectorValues(double *rh, double *th, double *tasic, double *tdac, double *t, double *hv)
{
    int status = -1;
    double v3_3, hvMon, hvOut, v1_2, v1_8, v3, v2_5, v3_31n, v1_651n, v1_8ana, v3_8ana, peltierCurrent, ntcTemperature;

    status = ReadEnvironmentValues(detectorHandle, rh, th, tasic, tdac, t, timeout);
    showError("ReadEnvironmentValues", status);

    status = ReadOperatingValues(detectorHandle, &v3_3, &hvMon, &hvOut, &v1_2, &v1_8, &v3, &v2_5, &v3_31n, &v1_651n, &v1_8ana, &v3_8ana, &peltierCurrent, &ntcTemperature, timeout);
    showError("ReadOperatingValues", status);
    *hv = hvOut;
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

   status = SetFrameFormatControl(detectorHandle, "Mono14", xResolution, yResolution, 0, 0, "One", "Off");
   showError( "SetFrameFormatControl", status);
   frameSize = xResolution * xResolution * 2;

   return status;
}

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
   if (command == COLLECT)
   {
      getImages(ival1, ival2);
   }

   else if (command == COLLECT_OFFSETS)
   {

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
   emit imageStarted(processingFilename, frameSize);
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

WindowsEvent *GigEDetector::getReturnBufferReadyEvent()
{
   return returnBufferReadyEvent;
}

WindowsEvent *GigEDetector::getShowImageEvent()
{
   return showImageEvent;
}

void GigEDetector::handleReducedDataCollection()
{
   emit executeGetImages();
}

void GigEDetector::handleExecuteGetImages()
{
   updateState(COLLECTING);
   emit executeAcquireImages();
}

void GigEDetector::handleExecuteOffsets()
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

LONG GigEDetector::collectOffsets()
{
   int status = -1;

   status = CollectOffsetValues(detectorHandle, 1000, collectDcTime);								// make sure to have stable operating conditions (high voltage, temperature, x-ray turned off)
   showError("CollectOffsetValues", status);

   return 0;
}

void GigEDetector::acquireImages()
{
   int status = -1;
   ULONGLONG framesAcquired;
   double durationSeconds = dataAcquisitionDuration/1000.0;
   ULONG frameCount = (durationSeconds/frameTime) + 0.5;
   ULONG frameTimeout = (ULONG)(frameTime * 2500.0);

   if( frameTimeout < 100 )
   {
      frameTimeout = 100;
   }

   status = AcquireFrames(detectorHandle, frameCount, &framesAcquired, frameTimeout);
   showError("AcquireFrames", status);
   if (mode == CONTINUOUS)
   {
      emit imageComplete(framesAcquired);
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

void GigEDetector::handleStop()
{
   updateState(READY);
   emit writeMessage("GigE DAQ finished.");
}

void GigEDetector::abort(bool restart)
{
   StopAcquisition(detectorHandle);
   updateState(READY);
}

void GigEDetector::updateState(DetectorState state)
{
   this->state = state;
   emit notifyState(state);
}

unsigned int GigEDetector::getXResolution()
{
   return xResolution;
}

unsigned int GigEDetector::getYResolution()
{
   return yResolution;
}

void GigEDetector::setXResolution(unsigned int xResolution)
{
   this->xResolution = xResolution;
}

void GigEDetector::setYResolution(unsigned int yResolution)
{
   this->yResolution = yResolution;
}

void GigEDetector::setMode(Mode mode)
{
   this->mode = mode;
}

void GigEDetector::setDirectory(QString directory)
{
   this->directory = directory;
}

void GigEDetector::setDataAcquisitionDuration(double dataAcquisitionDuration)
{
   this->dataAcquisitionDuration = dataAcquisitionDuration;
}

QString GigEDetector::getDirectory()
{
   return this->directory;
}

void GigEDetector::setPrefix(QString prefix)
{
   this->prefix = prefix;
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
   imageDest = (short *) malloc(imageSize * sizeof(short));
   image = (unsigned char *) malloc(imageSize * sizeof(unsigned char));
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

   sensorConfig.Gain = (HexitecGain)iniFile->getInt("Control-Settings/Gain");
   sensorConfig.Row_S1 = iniFile->getInt("Control-Settings/Row -> S1");
   sensorConfig.S1_Sph = iniFile->getInt("Control-Settings/S1 -> Sph");
   sensorConfig.Sph_S2 = iniFile->getInt("Control-Settings/Sph -> S2");
   sensorConfig.Vcal2_Vcal1 = iniFile->getInt("Control-Settings/VCAL2 -> VCAL1");
   sensorConfig.WaitClockCol = 1;
   sensorConfig.WaitClockRow = 8;

//   !!! Column configuration through the GigE.dll is failing and always returns 80 for xRes!
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


void GigEDetector::showError(const LPSTR context, long asError)
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