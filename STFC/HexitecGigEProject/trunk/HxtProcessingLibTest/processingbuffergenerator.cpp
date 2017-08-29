#include "processingbuffergenerator.h"
#include <QMutexLocker>
#include <QDebug>
#include <QThread>
#include <Windows.h>

ProcessingBufferGenerator::ProcessingBufferGenerator(ProcessingDefinition *processingDefinition, QObject *parent) : QObject(parent)
{
//   QMutexLocker locker(&mutex);

   imageProcessorList.clear();
   currentImageProcessor = NULL;
   this->processingDefinition = processingDefinition;
   qDebug() << "MAIN thread = " << QThread::currentThreadId();
}

void ProcessingBufferGenerator::enqueueImage(const char *name, int nRows, int nCols, ProcessingDefinition *processingDefinition)
{
   this->frameSize = nRows * nCols * sizeof(uint16_t);
   currentImageProcessor = new ImageProcessor(name, nRows, nCols, processingDefinition);
   connect(currentImageProcessor, SIGNAL(processingComplete(ImageProcessor *, long long)),
           this, SLOT(handleProcessingComplete(ImageProcessor *, long long)));
   imageProcessorList.append(currentImageProcessor);
   qDebug() << "IMAGE QUEUED: currentImageProcessor " << currentImageProcessor
            << "image queue length = " << imageProcessorList.length();
}

void ProcessingBufferGenerator::handleImageStarted(const char *path, int nRows, int nCols)
{
   QMutexLocker locker(&mutex);

   qDebug() << "ProcessingBufferGenerator::handleImageStarted. path: "
            << path << " frameSize " << nRows * nCols * sizeof(uint16_t);
   enqueueImage(path, nRows, nCols, processingDefinition);
}

void ProcessingBufferGenerator::handleTransferBufferReady(char *transferBuffer, unsigned long validFrames)
{
   bufferToProcess = (char *) malloc(frameSize * validFrames);
   memcpy(bufferToProcess, transferBuffer, frameSize * validFrames);
   currentImageProcessor->enqueueBuffer(bufferToProcess, validFrames);
// RETURN THE TRANSFER BUFFER FOR RE-USE!!!
   free(transferBuffer);
}

void ProcessingBufferGenerator::handleImageComplete(long long totalFramesAcquired)
{
   qDebug() << "ProcessingBufferGenerator::handleImageComplete. totalFramesAcquired: "
            << totalFramesAcquired << " buffer queue size: " << currentImageProcessor->imageItem->getBufferQueueSize();
   currentImageProcessor->imageAcquisitionComplete(totalFramesAcquired);
}

/*
void ProcessingBufferGenerator::handleConfigureProcessing(bool re_order,
                                             const char *gradientFilename,
                                             const char *interceptFilename,
                                             const char *processedFilename)
{
   qDebug() << "111 ProcessingBufferGenerator::handleConfigureProcessing() called";
   processingDefinition->setRe_order(re_order);
   processingDefinition->setGradientFilename((char *)gradientFilename);
   processingDefinition->setInterceptFilename((char *)interceptFilename);
   processingDefinition->setProcessedFilename((char *)processedFilename);
}
*/
void ProcessingBufferGenerator::handleConfigureProcessing(bool re_order, bool nextFrame,
                                                          int threshholdMode, int thresholdValue, const char *thresholdFilname,
                                                          const char *gradientFilename,
                                                          const char *interceptFilename,
                                                          const char *processedFilename)
{
   qDebug() << "222 ProcessingBufferGenerator::handleConfigureProcessing() thresholdfilname = " << thresholdFilname;
   processingDefinition->setRe_order(re_order);
   processingDefinition->setNextFrameCorrection(nextFrame);
   processingDefinition->setThresholdMode((ThresholdMode)threshholdMode);

   switch ((ThresholdMode)threshholdMode)
   {
      case SINGLE_VALUE:
         processingDefinition->setThresholdValue(thresholdValue);
         break;
      case THRESHOLD_FILE:
         processingDefinition->setThresholdPerPixel((char *)thresholdFilname);
         break;
      default:
         break;
   }
   processingDefinition->setGradientFilename((char *)gradientFilename);
   processingDefinition->setInterceptFilename((char *)interceptFilename);
   processingDefinition->setProcessedFilename((char *)processedFilename);
}

void ProcessingBufferGenerator::handleConfigureProcessing(bool energyCalibration, long long binStart, long long binEnd, double binWidth, bool totalSpectrum,
                                                          const char *gradientFilename,
                                                          const char *interceptFilename,
                                                          const char *processedFilename)
{
   processingDefinition->setEnergyCalibration(energyCalibration);
   processingDefinition->setBinStart(binStart);
   processingDefinition->setBinEnd(binEnd);
   processingDefinition->setBinWidth(binWidth);
   processingDefinition->setTotalSpectrum(totalSpectrum);
   processingDefinition->setGradientFilename((char *)gradientFilename);
   processingDefinition->setInterceptFilename((char *)interceptFilename);
   processingDefinition->setProcessedFilename((char *)processedFilename);

}

void ProcessingBufferGenerator::handleConfigureProcessing(int chargedSharingMode, int pixelGridOption,
                                                          const char *gradientFilename,
                                                          const char *interceptFilename,
                                                          const char *processedFilename)
{
   qDebug() << "8888 ProcessingBufferGenerator::handleConfigureProcessing()pixelGridOption= " << pixelGridOption;
   processingDefinition->setChargedSharingMode((ChargedSharingMode)chargedSharingMode);
   processingDefinition->setPixelGridSize(pixelGridOption);

   processingDefinition->setGradientFilename((char *)gradientFilename);
   processingDefinition->setInterceptFilename((char *)interceptFilename);
   processingDefinition->setProcessedFilename((char *)processedFilename);
}

void ProcessingBufferGenerator::handleProcessingComplete(ImageProcessor *completedImageProcessor, long long processedFrameCount)
{
   Sleep(500);
   imageProcessorList.removeOne(completedImageProcessor);
   free(completedImageProcessor);
   qDebug() << "ProcessingBufferGenerator::handleProcessingComplete: imageProcessorList.length()" << imageProcessorList.length();
}
