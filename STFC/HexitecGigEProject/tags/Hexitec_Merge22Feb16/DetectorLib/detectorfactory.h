#ifndef DETECTORFACTORY_H
#define DETECTORFACTORY_H

/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX

#include "gigedetector.h"
#include "imageacquirer.h"
#include "detectormonitor.h"

class DetectorFactory : public QObject
{
   Q_OBJECT
public:
   ~DetectorFactory();
   static DetectorFactory *instance(const QObject *parent = 0);
   GigEDetector *createGigEDetector(QString aspectFilename, QObject *parent);
   GigEDetector *getGigEDetector();
   DetectorMonitor *getDetectorMonitor();
   WindowsEvent *getBufferReadyEvent();
   WindowsEvent *getReturnBufferReadyEvent();
   WindowsEvent *getShowImageEvent();

protected:
   explicit DetectorFactory(const QObject *parent = 0);
private:
   static DetectorFactory *dfInstance;
   ImageAcquirer *imageAcquirer;
   GigEDetector *gigEDetector;
   DetectorMonitor *detectorMonitor;
   const QObject *parent;
   WindowsEvent *bufferReadyEvent;
   WindowsEvent *returnBufferReadyEvent;
   WindowsEvent *showImageEvent;
};

#endif // DETECTORFACTORY_H
