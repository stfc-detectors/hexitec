#include "hxtgenerator.h"
#include <QMutexLocker>
#include <QDebug>
#include <QThread>

HxtGenerator::HxtGenerator(ProcessingDefinition *processingDefinition, QObject *parent) : QObject(parent)
{
//   QMutexLocker locker(&mutex);

   imageProcessorQueue.clear();
   currentImageProcessor = NULL;
   this->processingDefinition = processingDefinition;
   qDebug() << "MAIN thread = " << QThread::currentThreadId();
}

void HxtGenerator::enqueueImage(const char *name, int frameSize, ProcessingDefinition *processingDefinition)
{
   if (currentImageProcessor != NULL)
   {
//      disconnect(???);
   }
   this->frameSize = frameSize;
   currentImageProcessor = new ImageProcessor(name, frameSize, processingDefinition);
   connect(currentImageProcessor, SIGNAL(processingComplete(ImageProcessor *, unsigned long long)),
           this, SLOT(handleProcessingComplete(ImageProcessor *, unsigned long long)));
   imageProcessorQueue.enqueue(currentImageProcessor);
   qDebug() << "IMAGE QUEUED: currentImageProcessor " << currentImageProcessor
            << "image queue size = " << imageProcessorQueue.size();
}

void HxtGenerator::handleImageStarted(const char *path, int frameSize)
{
   QMutexLocker locker(&mutex);

   qDebug() << "HxtGenerator::handleImageStarted. path: "
            << path << " frameSize " << frameSize;
   enqueueImage(path, frameSize, processingDefinition);
}

void HxtGenerator::handleTransferBufferReady(char *transferBuffer, unsigned long validFrames)
{
   bufferToProcess = (char *) malloc(frameSize * validFrames);
   memcpy(bufferToProcess, transferBuffer, frameSize * validFrames);
   currentImageProcessor->enqueueBuffer(bufferToProcess, validFrames);
// RETURN THE TRANSFER BUFFER FOR RE-USE!!!
}

void HxtGenerator::handleImageComplete(unsigned long long totalFramesAcquired)
{
   qDebug() << "HxtGenerator::handleImageComplete. totalFramesAcquired: "
            << totalFramesAcquired << " buffer queue size: " << currentImageProcessor->imageItem->getBufferQueueSize();
   currentImageProcessor->imageComplete(totalFramesAcquired);
}

void HxtGenerator::handleConfigureProcessing(int threshholdMode, int thresholdValue, uint16_t *thresholdPerPixel,
                                             const char *gradientFilename,
                                             const char *interceptFilename,
                                             const char *processedFilename)
{
   qDebug() << "HxtGenerator::handleConfigureProcessing() called";
   processingDefinition->setThresholdMode((ThresholdMode)threshholdMode);
   processingDefinition->setThresholdValue(thresholdValue);
   processingDefinition->setThresholdPerPixel(thresholdPerPixel);
   processingDefinition->setGradientFilename((char *)gradientFilename);
   processingDefinition->setInterceptFilename((char *)interceptFilename);
   processingDefinition->setProcessedFilename((char *)processedFilename);
}

void HxtGenerator::handleProcessingComplete(ImageProcessor *completedImageProcessor, unsigned long long processedFrameCount)
{
   qDebug() << "HxtGenerator::handleProcessingComplete";
   completedImageProcessor->disconnect();
//   imageProcessorQueue
}
