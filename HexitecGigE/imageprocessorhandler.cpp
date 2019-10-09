#include "imageprocessorhandler.h"
#include <QDebug>
#include <QThread>
ImageProcessorHandler::ImageProcessorHandler(ImageProcessor *imageProcessor) :
   QThread()
{
   this->imageProcessor = imageProcessor;
   start();
}

void ImageProcessorHandler::run()
{
   imageProcessor->setImageInProgress(true);
   imageProcessor->handleProcess();

   qDebug() << QThread::currentThreadId() << int(QThread::currentThreadId()) << "ImageProcessorHandler::run()ENDING!!! threadId: "<< QThread::currentThreadId();
   delete imageProcessor;
   emit processingComplete();
}
