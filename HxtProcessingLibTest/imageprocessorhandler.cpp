#include "imageprocessorhandler.h"
#include <QDebug>
#include <QThread>
//
#include <QTime>
ImageProcessorHandler::ImageProcessorHandler(ImageProcessor *imageProcessor) :
   QThread()
{
   this->imageProcessor = imageProcessor;
   start();
}

void ImageProcessorHandler::run()
{
    QTime qtTime;
    int procTime = 0;
    qtTime.restart();
//   qDebug()<< "In ImageProcessorHandler::run(), threadId: "<< QThread::currentThreadId();
   imageProcessor->setImageInProgress(true);
   imageProcessor->handleProcess();
   imageProcessor->freeAllocedMemory();
//   qDebug()<< "ImageProcessorHandler::run()ENDING!!! "<< QThread::currentThreadId();
   delete imageProcessor;
   emit processingComplete();
   procTime = qtTime.elapsed();
   qDebug() << "IPH procTime: " << (procTime) << " ms. CurrentTime: " << QTime::currentTime();
}
