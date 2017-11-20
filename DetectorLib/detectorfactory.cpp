#include "detectorfactory.h"
#include "windowsevent.h"
#include <Windows.h>

#include <iostream>
#include <QApplication>

using namespace std;

DetectorFactory *DetectorFactory::dfInstance = 0;

DetectorFactory::DetectorFactory(const QObject *parent)
{
   this->parent = parent;
}

DetectorFactory *DetectorFactory::instance()
{
   if (dfInstance == 0)
   {
      dfInstance = new DetectorFactory(0);
   }
   return dfInstance;
}

DetectorFactory *DetectorFactory::instance(const QObject *parent)
{
   if (dfInstance == 0)
   {
      dfInstance = new DetectorFactory(parent);
   }
   return dfInstance;
}

GigEDetector *DetectorFactory::createGigEDetector(string aspectFilename)
{
   QString aspectFilenameQ = QString::fromStdString(aspectFilename);
   return createGigEDetector(aspectFilenameQ);
}

GigEDetector *DetectorFactory::createGigEDetector(QString aspectFilename, QObject *parent)
{
   gigEDetector = new GigEDetector(aspectFilename, parent);
   imageAcquirer = new ImageAcquirer(this);
   imageAcquirer->setDetector(gigEDetector);
   bufferReadyEvent = gigEDetector->getBufferReadyEvent();
   returnBufferReadyEvent = gigEDetector->getReturnBufferReadyEvent();
   showImageEvent = gigEDetector->getShowImageEvent();
   notifyStateEvent = gigEDetector->getNotifyStateEvent();

   detectorMonitor = new DetectorMonitor(gigEDetector, NULL);

   return gigEDetector;
}

GigEDetector *DetectorFactory::getGigEDetector()
{
   return gigEDetector;
}

DetectorMonitor *DetectorFactory::getDetectorMonitor()
{
   return detectorMonitor;
}

WindowsEvent *DetectorFactory::getBufferReadyEvent()
{
   return bufferReadyEvent;
}

HANDLE *DetectorFactory::getTransferBufferReadyEvent()
{
   return transferBufferReadyEvent;
}

HANDLE *DetectorFactory::getReturnBufferReadyEvent()
{
   return returnBufferReadyEvent;
}

WindowsEvent *DetectorFactory::getShowImageEvent()
{
   return showImageEvent;

}

HANDLE *DetectorFactory::getNotifyStateEvent()
{
   return notifyStateEvent;
}

DetectorFactory::~DetectorFactory()
{
   dfInstance = 0;
}
