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

   qDebug()<< "ImageProcessorHandler::run()ENDING!!! "<< QThread::currentThreadId();
   delete imageProcessor;
   emit processingComplete();
}
