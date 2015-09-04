#include <QDebug>
#include <QThread>
/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX
#define STR_LENGTH 1024
//
#include <QImage>

#include "windowsevent.h"
//#include "GigE_extended.h"
#include "gigedetector.h"
#include "detectorfactory.h"

static PUCHAR bufferReady;
static PUCHAR returnBuffer;
static ULONG validFrames;
HexitecOperationMode operationMode = { AS_CONTROL_DISABLED, AS_CONTROL_DISABLED,
                                                AS_CONTROL_DISABLED, AS_CONTROL_ENABLED,
                                                AS_CONTROL_DISABLED, AS_CONTROL_ENABLED,
                                                AS_CONTROL_DISABLED, AS_CONTROL_DISABLED,
                                                AS_CONTROL_DISABLED, AS_CONTROL_DISABLED,
                                                AS_CONTROL_DISABLED, 0 };
HexitecSystemConfig	systemConfig = { 2, 10, AS_HEXITEC_ADC_SAMPLE_FALLING_EDGE, 4 };

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
   qDebug() << "aspectFilename = " << aspectFilename;
   aspectFilename = "C:/karen/STFC/Technical/Detector/aspectGigE.ini";
   this->aspectFilename = aspectFilename;
   gigEDetectorThread = new QThread();
   gigEDetectorThread->start();
   moveToThread(gigEDetectorThread);

   readIniFile(this->aspectFilename);

//   returnBufferReadyEvent = DetectorFactory::instance()->getReturnBufferReadyEvent();

   xRes = 80;
   yRes = 80;
   vCal = 0.5;
   uMid = 1.0;
   detCtrl = 0;
   targetTemperature = 20.0;
   hvSetPoint = 0;

   qRegisterMetaType<GigEDetector::DetectorCommand>("GigE::DetectorCommand");
   qRegisterMetaType<GigEDetector::DetectorState>("GigE::DetectorState");
   qRegisterMetaType<GigEDetector::DetectorState>("GigE::Mode");
   qRegisterMetaType<HANDLE>("HANDLE");
   connectUp(parent);

   qDebug() <<"GigEDetector Constructor";
   initialiseConnection();
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
//   returnBufferReadyEvent->connect1(this, SLOT(handleReturnBufferReady()));
   showImageEvent = new WindowsEvent(HEXITEC_SHOW_IMAGE, true);
   showImageEvent->connect1(parent, SLOT(handleShowImage()));

   /*   startTriggerEvent = new WindowsEvent(HEXITEC_START_TRIGGER, true);
   triggeredEvent = new WindowsEvent(HEXITEC_TRIGGERED, true);
   triggeredReducedDataEvent = new WindowsEvent(HEXITEC_TRIGGERED_REDUCED_DATA, true);
   stopEvent = new WindowsEvent(HEXITEC_STOP, true);
   saveBufferEvent = new WindowsEvent(HEXITEC_SAVE_BUFFER, true);
   stopGrabEvent = new WindowsEvent(HEXITEC_STOP_GRAB, true, true);

   triggeredEvent->connect1(this, SLOT(handleTriggered()));
   triggeredReducedDataEvent->connect1(this, SLOT(handleTriggeredReducedData()));
   stopEvent->connect1(this, SLOT(handleStop()));
*/
   connect(this , SIGNAL(executeReturnBufferReady(unsigned char *)), this, SLOT(handleReturnBufferReady()));
   connect(this, SIGNAL(executeGetImages()), this, SLOT(handleExecuteGetImages()));
//   connect(this, SIGNAL(notifyStop()), this, SLOT(handleStop()));
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
   qDebug() << "handleReturnBufferReady, address" << returnBuffer;
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

int GigEDetector::initialiseConnection()
{
   LONG status = -1;
   CONST LPSTR deviceDescriptor = "";
   ULONG pleoraErrorCodeStrLen = STR_LENGTH;
   ULONG pleoraErrorDescriptionLen = STR_LENGTH;
   ULONG	pleoraErrorCode = -1;
   CHAR pleoraErrorCodeStr[STR_LENGTH] = {0};
   CHAR pleoraErrorDescription[STR_LENGTH] = {0};
   GigEDeviceInfoStr deviceInfo = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
   updateState(INITIALISING);

   qDebug() <<"InitDevice";
   status = InitDevice(&detectorHandle, deviceDescriptor, &pleoraErrorCode, pleoraErrorCodeStr,
                             &pleoraErrorCodeStrLen, pleoraErrorDescription, &pleoraErrorDescriptionLen);
   qDebug() <<"InitDevice status" << status;
   showError("InitDevice", status);

   status = GetDeviceInformation(detectorHandle, &deviceInfo);
   showError("GetDeviceInformation", status);

   if( !status )
   {
      qDebug() <<"\tVendor:" << deviceInfo.Vendor;
      qDebug() <<"\tModel:" << deviceInfo.Model;
      qDebug() <<"\tManufacturerInfo:" << deviceInfo.ManufacturerInfo;
      qDebug() <<"\tSerialNumber:" <<deviceInfo.SerialNumber;
      qDebug() <<"\tUserId:" << deviceInfo.UserId;
      qDebug() <<"\tMacAddress:" << deviceInfo.MacAddress;
      qDebug() <<"\tIpAddress:" << deviceInfo.IpAddress;
      qDebug() <<"\tNetMask:" << deviceInfo.NetMask;
      qDebug() <<"\tGateWay:" << deviceInfo.GateWay;
   }

   status = CloseSerialPort(detectorHandle);
   showError( "CloseSerialPort", status);

   status = ClosePipeline(detectorHandle);
   showError( "ClosePipeline", status);

   status = CloseStream(detectorHandle);
   showError( "CloseStream", status);

   status = OpenSerialPort(detectorHandle, 2, 2048, 1, 0x0d );
   showError("OpenSerialPort", status);

   frameTime = 0;

   status = OpenStream(detectorHandle);
   showError("OpenStream", status);
   xRes = 80;
   yRes = 80;
   framesPerBuffer = 100;

   status = ConfigureDetector(detectorHandle, &sensorConfig, &operationMode, &systemConfig,
                              &xRes, &yRes, &frameTime, &collectDcTime, 1000);
   showError( "ConfigureDetector", status);
   if (!status)
   {
      qDebug() <<"Configure Detector - width:" << xRes;
      qDebug() <<"Configure Detector - height:" << yRes;
      qDebug() << "Configure Detector - frameTime:" << frameTime ;
      qDebug() <<"Configure Detector - collectDcTime:" << collectDcTime ;
   }
   status = setImageFormat(xRes, yRes);
   qDebug() <<"setImageFormat: xRes, yRes, status " << xRes << yRes << status;
   status = CreatePipeline(detectorHandle, 512, 100, framesPerBuffer);
   qDebug() <<"CreatePipeline(): framesPerBufer, status " << framesPerBuffer << status;

   qDebug() << "!!!!!!!!!!!!!!!!!GigEDetector::InitialiseConnection and configure DONE!!!!!!!!!!!!!!!";

   RegisterTransferBufferReadyCallBack(detectorHandle, bufferCallBack);

   updateState(INITIALISED);
   updateState(READY);

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

   return status;
}

int GigEDetector::getDetectorValues(double *rh, double *th, double *tasic, double *tadc, double *t, double *hv)
{
    int status = -1;
    double v3_3, hvMon, hvOut, v1_2, v1_8, v3, v2_5, v3_31n, v1_651n, v1_8ana, v3_8ana, peltierCurrent, ntcTemperature;

    status = ReadEnvironmentValues(detectorHandle, rh, th, tasic, tadc, t, timeout);
    showError("ReadEnvironmentValues", status);

    status = ReadOperatingValues(detectorHandle, &v3_3, &hvMon, &hvOut, &v1_2, &v1_8, &v3, &v2_5, &v3_31n, &v1_651n, &v1_8ana, &v3_8ana, &peltierCurrent, &ntcTemperature, timeout);
    showError("ReadEnvironmentValues", status);
    *hv = hvOut;

    return status;
}

int GigEDetector::setImageFormat(unsigned long xResolution, unsigned long yResolution)
{
   LONG status = -1;

   status = SetFrameFormatControl(detectorHandle, "Mono14", xResolution,yResolution, 0, 0, "One", "Off");
   showError( "SetFrameFormatControl", status);

   return status;
}
/*
int GigEDetector::configure(unsigned long xResolution, unsigned long yResolution)
{
   LONG status = -1;
   double dataAcquisitionDuration;

//   mode = GIGE_DEFAULT;
   mode = CONTINUOUS;
   qDebug() << "!!!!!!!!!!!!!!!!!GigEDetector::configure!!!!!!!!!!!!!!!!!!!!";

   if (mode == GIGE_DEFAULT)
   {
      dataAcquisitionDuration = 1.0;
      framesPerBuffer = (unsigned long) (1000.0/ frameTime);
   }
   else
   {
      framesPerBuffer = 100;
      count = 1;
   }

   status = ClosePipeline(detectorHandle);
   status = setImageFormat(xResolution, yResolution);
   status = CreatePipeline(detectorHandle, 512, 100, framesPerBuffer);

   qDebug() << "!!!!!!!!!!!!!!!!!GigEDetector::configure DONE!!!!!!!!!!!!!!!";
   return status;
}
*/
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
      qDebug() << "command == COLLECT_OFFSETS";
//      getOffsets();
   }
/*   else if (command == TRIGGER)
   {
      triggerCollection();
   }
   else if (command == CONNECT)
   {
      connectUp();
   }

   else if (command == CONFIGURE)
   {
      configure(ival1, ival2);
   }
      else
      {
         // Already configured for the requested mode and quadrant.
         emit notifyMode(mode);
      }
   }
   /*
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
   */
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
   /*
   else if (command == STOP_TRIGGER)
   {
      abort(false);
      updateState(TRIGGERING_STOPPED);
   }*/
}

void GigEDetector::getImages(int count, int ndaq)
{
   this->count = count;
   setGetImageParams();

   qDebug() << "ndaq, offsetsOn" <<  ndaq << offsetsOn;

   if (ndaq == 0 && offsetsOn)
   {
      updateState(OFFSETS_PREP);
      emit prepareForOffsets();
   }
   else
   {
//      emit executeGetImages();
      handleReducedDataCollection();
   }

}

void GigEDetector::setGetImageParams()
{
   QString path;

//   streaming = 1;
   path = directory + "/" + prefix;
   qDebug() << "setGetImageParams() path = " << path;
   /*
   if (timestampOn)
   {
      path += QDateTime::currentDateTime().toString("yyMMdd_hhmmss") + "_reduced";
   }
*/
   path.replace(QString("/"), QString("\\"));
   sprintf_s(pathString, "%s", path.toUtf8().data());

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
   qDebug() << "handleReducedDataCollection";
   emit executeGetImages();
}

void GigEDetector::handleExecuteGetImages()
{
   updateState(COLLECTING);
//   this->count = count;
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

void GigEDetector::offsetsDialogAccepted()
{
//   collectOffsets();
   qDebug() << "Call collectOffsets() here!!!";
}

LONG GigEDetector::collectOffsets()
{
   int status = -1;

   qDebug() << "Get GigE to collect and load up offsets here!!!";
   status = CollectOffsetValues(detectorHandle, 1000, collectDcTime);								// make sure to have stable operating conditions (high voltage, temperature, x-ray turned off)
   showError("CollectOffsetValues", status);

   return 0;
}

void GigEDetector::acquireImages()
{
   int status = -1;
   ULONGLONG framesAcquired;
   double durationSeconds = dataAcquisitionDuration/1000.0;
   ULONG frameCount = count * ((durationSeconds/frameTime) + 0.5);
   ULONG frameTimeout = (ULONG)(frameTime * 2500.0);

   if( frameTimeout < 100 )
   {
      frameTimeout = 100;
   }
   qDebug() <<"frameTime" << frameTime << "count" << count << "frames per buffer" << framesPerBuffer;

   qDebug() <<"frameCount" << frameCount << "durationSeconds" << durationSeconds;
   status = AcquireFrames(detectorHandle, frameCount, &framesAcquired, frameTimeout);
   qDebug() <<"Acquire frames returned status " << status << "Acquired frames " << framesAcquired;
   updateState(READY);
}

void GigEDetector::handleStop()
{
   updateState(READY);
   emit writeMessage("GigE DAQ finished.");
}

void GigEDetector::abort(bool restart)
{
   qDebug() << "*** data acquisition stopping ***, threadId" << QThread::currentThreadId();
   StopAcquisition(detectorHandle);
   qDebug() << "data acquisition stopped.";
   updateState(READY);
}

void GigEDetector::updateState(DetectorState state)
{
   this->state = state;
//   qDebug() <<"DetectorState state: " << state;
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
   qDebug() <<"setDataAcquisitionDuration: " << dataAcquisitionDuration;
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
   //qDebug() << "GigEDetector Setting timestampOn to " << timestampOn;
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
/*
QStringList GigEDetector::getModes()
{
   return modes;
}

QStringList GigEDetector::getReducedDataModes()
{
   return reducedDataModes;
}
*/
void GigEDetector::enableDarks()
{
   offsetsOn = true;
}

void GigEDetector::disableDarks()
{
   offsetsOn = false;
}
/*
QSize GigEDetector::getSize()
{
   return QSize(xRes, yRes);
}
*/

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
   qDebug() << "Getting an image from buffer at address: " << buffer;
   imageDest = (short *) malloc(imageSize * sizeof(short));
   image = (unsigned char *) malloc(imageSize * sizeof(unsigned char));
   memcpy(imageDest, buffer, segmentSize);
   returnBuffer = buffer;
   emit executeReturnBufferReady(returnBuffer);

//   returnBufferReadyEvent->SetEvent1();
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

   status = 0;
   iniFile = new IniFile(aspectFilename);
//   directory = iniFile->getString("Controls/Toplevel Path");
   directory = "C:/karen/STFC/Technical/DSoFt_Images";
//   prefix = iniFile->getString("Controls/Prefix");
   prefix = "Hexitec_";
   qDebug() << "aspectFilename:" << aspectFilename;

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


void GigEDetector::showError(const LPSTR context, long asError)
{
   LONG result = 1;
   LONG sysError = 0;
   ULONG pleoraErrorCodeStrLen = STR_LENGTH;
   ULONG pleoraErrorDescriptionLen = STR_LENGTH;
   CHAR asErrorMessage[STR_LENGTH] = { 0 };
   CHAR sysErrorMessage[STR_LENGTH] = { 0 };
   ULONG	pleoraErrorCode = 0;
   CHAR	pleoraErrorCodeStr[STR_LENGTH] = { 0 };
   CHAR	pleoraErrorDescription[STR_LENGTH] = { 0 };

   sysError = GetAsErrorMsg( asError, asErrorMessage, STR_LENGTH );

   if( sysError )
   {
      sysError = GetSystemErrorMsg( sysError, sysErrorMessage, STR_LENGTH );
      qDebug() << "\n%s\n\tSystem Error" << context << sysError << "occured trying to get aSpect error message for aSpect error: " << sysErrorMessage << asError;
      return;
   }
   else if( asError )
   {
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
      qDebug() << "aSpect Result: " << context << asErrorMessage;
      qDebug() << "Pleora Result Code:" << pleoraErrorCode;
      qDebug() << "Pleora Result Code String:" << pleoraErrorCodeStr;
      qDebug() << "Pleora Result Description:" << pleoraErrorDescription;
   }

}
