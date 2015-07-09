#include <QDebug>
#include "windowsevent.h"
#include "detectorfactory.h"

DetectorFactory *DetectorFactory::dfInstance = 0;

DetectorFactory::DetectorFactory(const QObject *parent)
{
   this->parent = parent;
}

DetectorFactory *DetectorFactory::instance(const QObject *parent)
{
   if (dfInstance == 0)
   {
      dfInstance = new DetectorFactory(parent);
   }
   return dfInstance;
}

GigEDetector *DetectorFactory::createGigEDetector(QObject *parent)
{
   gigEDetector = new GigEDetector(parent);
   imageAcquirer = new ImageAcquirer(this);
   imageAcquirer->setDetector(gigEDetector);
   bufferReadyEvent = gigEDetector->getBufferReadyEvent();
   //stopDAQEvent = gigEDetector->getStopDAQEvent();
   showImageEvent = gigEDetector->getShowImageEvent();
//   detectorMonitor = new DetectorMonitor(gigEDetector, loggingInterval);
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

/*WindowsEvent *DetectorFactory::getStopDAQEvent()
{
   return stopDAQEvent;
}*/

WindowsEvent *DetectorFactory::getShowImageEvent()
{
   return showImageEvent;

}

DetectorFactory::~DetectorFactory()
{
   dfInstance = 0;
}
