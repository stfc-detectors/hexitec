#include "hxtgenerator.h"
#include <QDebug>
#include <QThread>

HxtGenerator::HxtGenerator(ProcessingDefinition *processingDefinition, QObject *parent) : QObject(parent)
{
   imageProcessorQueue.clear();
   currentImageProcessor = NULL;
   this->processingDefinition = processingDefinition;
   qDebug() << "MAIN thread = " << QThread::currentThreadId();

}

void HxtGenerator::enqueueImage(const char *name, int frameSize, ProcessingDefinition *processingDefinition)
{
   this->frameSize = frameSize;
   currentImageProcessor = new ImageProcessor(name, frameSize, processingDefinition);
   imageProcessorQueue.enqueue(currentImageProcessor);
   qDebug() << "IMAGE QUEUED";
//   connect(this, SIGNAL(enqueueBuffer(char*,ulong)),
//currentImageProcessor,SLOT(handleEnqueueBuffer(char*,ulong)));
//   connect(this, SIGNAL(imageComplete(unsigned long long)),
//           currentImageProcessor, SLOT(handleImageComplete(unsigned long long)));
}

void HxtGenerator::handleImageStarted(const char *path, int frameSize)
{
   qDebug() << "HxtGenerator::handleImageStarted. path: "
            << path << " frameSize " << frameSize;
   enqueueImage(path, frameSize, processingDefinition);
}

void HxtGenerator::handleTransferBufferReady(char *transferBuffer, unsigned long validFrames)
{
   qDebug() << "HxtGenerator::handleTransferBufferReady. validFrames: " << validFrames;
   bufferToProcess = (char *) malloc(frameSize);
   memcpy(bufferToProcess, transferBuffer, frameSize);
   currentImageProcessor->handleEnqueueBuffer(bufferToProcess, validFrames);
//   emit enqueueBuffer (bufferToProcess, validFrames);
}

void HxtGenerator::handleImageComplete(unsigned long long totalFramesAcquired)
{
   qDebug() << "HxtGenerator::handleImageComplete. totalFramesAcquired: "
            << totalFramesAcquired << " buffer queue size: " << currentImageProcessor->imageItem->getBufferQueueSize();
   currentImageProcessor->handleImageComplete(totalFramesAcquired);
//   emit imageComplete(totalFramesAcquired);
   disconnect(currentImageProcessor);
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
