#include "imageprocessorhandler.h"
#include <QDebug>
#include <QThread>
ImageProcessorHandler::ImageProcessorHandler(ImageProcessor *imageProcessor) :
   QThread()
{
   qDebug()<< "In ImageProcessorHandler CONSTRUCTOR threadId: "<< QThread::currentThreadId();
   this->imageProcessor = imageProcessor;
   start();
   qDebug()<< "In ImageProcessorHandler CONSTRUCTOR started - returning ";
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
