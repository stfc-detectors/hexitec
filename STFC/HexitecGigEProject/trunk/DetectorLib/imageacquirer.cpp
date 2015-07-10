#include <QDebug>
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
   connect(this->detector, SIGNAL(executeAcquireImages()), this, SLOT(handleExecuteAcquireImages()));
}

void ImageAcquirer::handleExecuteAcquireImages()
{
   qDebug() <<"handleAcquireImages called, count: In threadId " << QThread::currentThreadId();
   detector->acquireImages();
}
