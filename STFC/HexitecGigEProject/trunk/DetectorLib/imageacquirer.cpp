#include <QThread>

#include "imageacquirer.h"

ImageAcquirer::ImageAcquirer(QObject *parent)
{
   imageAcquirerThread = new QThread();
   imageAcquirerThread->start();
   moveToThread(imageAcquirerThread);
}

ImageAcquirer::~ImageAcquirer()
{
   QThread::currentThread()->exit();
}

void ImageAcquirer::setDetector(GigEDetector *detector)
{
   this->detector = detector;
   connect(this->detector, SIGNAL(executeAcquireImages(bool)), this, SLOT(handleExecuteAcquireImages(bool)));
}

void ImageAcquirer::handleExecuteAcquireImages(bool startOfImage)
{
   detector->acquireImages(startOfImage);
}
