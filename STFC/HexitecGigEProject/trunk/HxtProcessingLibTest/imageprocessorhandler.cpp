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
   qDebug()<< "In ImageProcessorHandler::run(), threadId: "<< QThread::currentThreadId();
   imageProcessor->setImageInProgress(true);
   imageProcessor->handleProcess();
   imageProcessor->freeAllocedMemory();
   qDebug()<< "ImageProcessorHandler::run()ENDING!!! "<< QThread::currentThreadId();
   delete imageProcessor;
   emit processingComplete();
}
