#include "processingbuffergenerator.h"
#include <QMutexLocker>
#include <QDebug>
#include <QThread>

ProcessingBufferGenerator::ProcessingBufferGenerator(ProcessingDefinition *processingDefinition, QObject *parent) : QObject(parent)
{
//   QMutexLocker locker(&mutex);

   imageProcessorList.clear();
   currentImageProcessor = NULL;
   this->processingDefinition = processingDefinition;
   qDebug() << "MAIN thread = " << QThread::currentThreadId();
}

void ProcessingBufferGenerator::enqueueImage(const char *name, int frameSize, ProcessingDefinition *processingDefinition)
{
   this->frameSize = frameSize;
   currentImageProcessor = new ImageProcessor(name, frameSize, processingDefinition);
   connect(currentImageProcessor, SIGNAL(processingComplete(ImageProcessor *, unsigned long long)),
           this, SLOT(handleProcessingComplete(ImageProcessor *, unsigned long long)));
   imageProcessorList.append(currentImageProcessor);
   qDebug() << "IMAGE QUEUED: currentImageProcessor " << currentImageProcessor
            << "image queue length = " << imageProcessorList.length();
}

void ProcessingBufferGenerator::handleImageStarted(const char *path, int frameSize)
{
   QMutexLocker locker(&mutex);

   qDebug() << "ProcessingBufferGenerator::handleImageStarted. path: "
            << path << " frameSize " << frameSize;
   enqueueImage(path, frameSize, processingDefinition);
}

void ProcessingBufferGenerator::handleTransferBufferReady(char *transferBuffer, unsigned long validFrames)
{
   bufferToProcess = (char *) malloc(frameSize * validFrames);
   memcpy(bufferToProcess, transferBuffer, frameSize * validFrames);
   currentImageProcessor->enqueueBuffer(bufferToProcess, validFrames);
// RETURN THE TRANSFER BUFFER FOR RE-USE!!!
   free(transferBuffer);
}

void ProcessingBufferGenerator::handleImageComplete(unsigned long long totalFramesAcquired)
{
   qDebug() << "ProcessingBufferGenerator::handleImageComplete. totalFramesAcquired: "
            << totalFramesAcquired << " buffer queue size: " << currentImageProcessor->imageItem->getBufferQueueSize();
   currentImageProcessor->imageComplete(totalFramesAcquired);
}

void ProcessingBufferGenerator::handleConfigureProcessing(bool re_order,
                                             const char *gradientFilename,
                                             const char *interceptFilename,
                                             const char *processedFilename)
{
   qDebug() << "ProcessingBufferGenerator::handleConfigureProcessing() called";
   processingDefinition->setRe_order(re_order);
   processingDefinition->setGradientFilename((char *)gradientFilename);
   processingDefinition->setInterceptFilename((char *)interceptFilename);
   processingDefinition->setProcessedFilename((char *)processedFilename);
}

void ProcessingBufferGenerator::handleConfigureProcessing(int threshholdMode, int thresholdValue, uint16_t *thresholdPerPixel,
                                             const char *gradientFilename,
                                             const char *interceptFilename,
                                             const char *processedFilename)
{
   qDebug() << "ProcessingBufferGenerator::handleConfigureProcessing() called";
   processingDefinition->setThresholdMode((ThresholdMode)threshholdMode);
   processingDefinition->setThresholdValue(thresholdValue);
   processingDefinition->setThresholdPerPixel(thresholdPerPixel);
   processingDefinition->setGradientFilename((char *)gradientFilename);
   processingDefinition->setInterceptFilename((char *)interceptFilename);
   processingDefinition->setProcessedFilename((char *)processedFilename);
}

void ProcessingBufferGenerator::handleConfigureProcessing(bool energyCalibration, unsigned long long binStart, unsigned long long binEnd, unsigned long long binWidth, bool totalSpectrum,
                                                          const char *gradientFilename,
                                                          const char *interceptFilename,
                                                          const char *processedFilename)
{
   qDebug() << "Configure Energy Calibration: binStart, binEnd " << binStart << ", " << binEnd;
   processingDefinition->setEnergyCalibration(energyCalibration);
   processingDefinition->setBinStart(binStart);
   processingDefinition->setBinEnd(binEnd);
   processingDefinition->setBinWidth(binWidth);
   processingDefinition->setTotalSpectrum(totalSpectrum);
   processingDefinition->setGradientFilename((char *)gradientFilename);
   processingDefinition->setInterceptFilename((char *)interceptFilename);
   processingDefinition->setProcessedFilename((char *)processedFilename);

}

void ProcessingBufferGenerator::handleProcessingComplete(ImageProcessor *completedImageProcessor, unsigned long long processedFrameCount)
{
   imageProcessorList.removeOne(completedImageProcessor);
   free(completedImageProcessor);
   qDebug() << "ProcessingBufferGenerator::handleProcessingComplete: imageProcessorList.length()" << imageProcessorList.length();
}
