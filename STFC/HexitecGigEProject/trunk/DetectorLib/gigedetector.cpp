#include <QDebug>
#include <QThread>
/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX
//
#include <QImage>

#include "windowsevent.h"
#include "GigE_extended.h"
#include "gigedetector.h"
#include "detectorfactory.h"

static PUCHAR bufferReady;

static void __cdecl bufferCallBack(PUCHAR transferBuffer, ULONG imageCount)
{
   bufferReady = transferBuffer;
   WindowsEvent *bufferReadyEvent = DetectorFactory::instance()->getBufferReadyEvent();
   WindowsEvent *showImageEvent = DetectorFactory::instance()->getShowImageEvent();

   bufferReadyEvent->SetEvent1();
   showImageEvent->SetEvent1();
}

GigEDetector::GigEDetector(const QObject *parent)
{
   gigEDetectorThread = new QThread();
   gigEDetectorThread->start();
   moveToThread(gigEDetectorThread);
   xRes = 80;
   yRes = 80;

   qRegisterMetaType<GigEDetector::DetectorCommand>("GigE::DetectorCommand");
   qRegisterMetaType<GigEDetector::DetectorState>("GigE::DetectorState");
   qRegisterMetaType<GigEDetector::DetectorState>("GigE::Mode");
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
   connect(this, SIGNAL(executeGetImages(int, int)), this, SLOT(handleExecuteGetImages(int, int)));
//   connect(this, SIGNAL(notifyStop()), this, SLOT(handleStop()));
}

void GigEDetector::handleBufferReady()
{
   emit executeBufferReady(bufferReady);
}

PUCHAR GigEDetector::getBufferReady()
{
   return bufferReady;
}

void GigEDetector::handleReturnBufferReady(PUCHAR transferBuffer)
{
   qDebug() << "handleReturnBufferReady(PUCHAR transferBuffer), address" << transferBuffer;
   GigE::ReturnBuffer(GigE::detectorHandle, transferBuffer);
}

int GigEDetector::initialiseConnection()
{
   LONG status = -1;
   CONST LPSTR deviceDescriptor = "";

   updateState(INITIALISING);
   status = GigE::InitDevice(&GigE::detectorHandle,
                       deviceDescriptor);
   qDebug() <<"InitialiseDevice returned status " << status;

   status = GigE::OpenStream(GigE::detectorHandle);

   status = GigE::OpenSerialPort(GigE::detectorHandle, 2, 2048, 1, 0x0d);

   GigE::RegisterTransferBufferReadyCallBack(GigE::detectorHandle, bufferCallBack);

   updateState(INITIALISED);
   updateState(READY);

   return status;
}

int GigEDetector::terminateConnection()
{
   LONG status = -1;

   status = GigE::CloseSerialPort(GigE::detectorHandle);
   qDebug() <<"CloseSerialPort returned status " << status;

   status = GigE::ClosePipeline(GigE::detectorHandle);
   qDebug() <<"ClosePipeline returned status " << status;

   status = GigE::CloseStream(GigE::detectorHandle);
   qDebug() <<" CloseStream returned status " << status;

   status = GigE::ExitDevice(GigE::detectorHandle);

//   status = GetLastResult(detectorHandle);
   qDebug() <<"ExitDevice returned status " << status;

   return status;
}

int GigEDetector::setImageFormat(unsigned long xResolution, unsigned long yResolution)
{
   LONG status = -1;

   //status = GigE::SetImageFormatControl(GigE::detectorHandle, "Mono14", xResolution, yResolution, 0, 0, "One", "Off");

   qDebug() <<"SetImageFormatControl xResolution, yResolution " << xResolution << "," << yResolution;
   status = GigE::SetImageFormatControl(GigE::detectorHandle, "Mono14", xResolution, yResolution, 0, 0, "One", "Off");
   qDebug() <<"SetImageFormatControl returned status " << status;

   return status;
}

int GigEDetector::configure(unsigned long xResolution, unsigned long yResolution)
{
   LONG status = -1;
   double dataAcquisitionDuration;
   double frameTime = 1.0/400.0;

   mode = GIGE_DEFAULT;

   qDebug() << "CONFIGURING mode " << mode;
   if (mode == GIGE_DEFAULT)
   {
      dataAcquisitionDuration = 1;
   }
   else
   {
      dataAcquisitionDuration = this->dataAcquisitionDuration;
   }
   qDebug() << "CONFIGURING daqd " << dataAcquisitionDuration;
   framesPerBuffer = (unsigned long) dataAcquisitionDuration / frameTime;

   status = GigE::ClosePipeline(GigE::detectorHandle);
   status = setImageFormat(xResolution, yResolution);
   qDebug() << "CONFIGURING CreatePipeline framesPerBuffer:" <<  framesPerBuffer;
   status = GigE::CreatePipeline(GigE::detectorHandle, 512, 100, framesPerBuffer);
   qDebug() <<"CreatePipeline returned status " << status;

   return status;
}

void GigEDetector::handleExecuteCommand(GigEDetector::DetectorCommand command, int ival1, int ival2)
{
   int status;

//   qDebug() << "Received command" << command << ival1 << ival2 ;
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
      qDebug() << "Detector collect images with values " << ival1 << ", " << ival2;
      getImages(ival1, ival2);
   }
/*
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
   */
   else if (command == CONFIGURE)
   {
      qDebug() << "Detector command = CONFIGURE" << ival1 << ival2;
      configure(ival1, ival2);
   }
/*      else
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
   emit executeGetImages(count, ndaq);
}

WindowsEvent *GigEDetector::getBufferReadyEvent()
{
   return bufferReadyEvent;
}

WindowsEvent *GigEDetector::getShowImageEvent()
{
   return showImageEvent;
}

void GigEDetector::handleExecuteGetImages(int count, int ndaq)
{
   updateState(COLLECTING);
   this->count = count;
   qDebug() <<"handleExecuteGetImages called, count: " << count <<" In threadId " << QThread::currentThreadId();
   emit executeAcquireImages();
}

void GigEDetector::acquireImages()
{
   int status;
   ULONGLONG imagesAcquired;
   ULONG frameCount = count * framesPerBuffer;

   qDebug() <<"acquireImages called, count and frameCount: " << count << " " << frameCount;
   status = GigE::AcquireImages(GigE::detectorHandle, frameCount, &imagesAcquired);
   qDebug() <<"Acquire images returned status " << status;
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
   GigE::StopAcquisition(GigE::detectorHandle);
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
   handleReturnBufferReady(buffer);
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

