#include <QDebug>
#include "windowsevent.h"
#include "parameters.h"
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
   QString aspectFilename = Parameters::aspectIniFilename;

   QSettings settings(QSettings::UserScope, "TEDDI", "2Easy");
   if (settings.contains("aspectIniFilename"))
   {
      aspectFilename = settings.value("aspectIniFilename").toString();
   }

   gigEDetector = new GigEDetector(aspectFilename, parent);
   imageAcquirer = new ImageAcquirer(this);
   imageAcquirer->setDetector(gigEDetector);
   bufferReadyEvent = gigEDetector->getBufferReadyEvent();
   returnBufferReadyEvent = gigEDetector->getReturnBufferReadyEvent();
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

WindowsEvent *DetectorFactory::getReturnBufferReadyEvent()
{
   return returnBufferReadyEvent;
}

WindowsEvent *DetectorFactory::getShowImageEvent()
{
   return showImageEvent;

}

DetectorFactory::~DetectorFactory()
{
   dfInstance = 0;
}
