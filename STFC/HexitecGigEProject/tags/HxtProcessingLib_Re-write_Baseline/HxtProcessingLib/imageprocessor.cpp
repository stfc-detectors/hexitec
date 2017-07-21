#include "imageprocessor.h"
#include <QThread>
#include <QDebug>
#include <Windows.h>
#include <iostream>
#include <fstream>

ImageProcessor::ImageProcessor(const char *name, int frameSize, ProcessingDefinition *processingDefinition)
{
   imageProcessorThread = new QThread();
   imageProcessorThread->start();
   moveToThread(imageProcessorThread);
   this->frameSize = frameSize;
   imageItem = new ImageItem(name, frameSize);
   this->processingDefinition = processingDefinition;
   setImageInProgress(true);
   connect(this, SIGNAL(process()), this, SLOT(handleProcess()));
   emit process();
}

ImageProcessor::~ImageProcessor()
{
   QThread::currentThread()->exit();
}

void ImageProcessor::processThresholdNone(GeneralFrameProcessor *fp, uint16_t *result, const char* filename)
{
   unsigned long validFrames = 0;
   char *bufferStart;
   char *frameIterator;

   filename = "C://karen//STFC//Technical//PLTest//re_order.bin";
   qDebug() << "ImageProcessor::process() ThresholdMode::NONE" << frameSize;
   while (inProgress || (imageItem->getBufferQueueSize() > 0))
   {
      while (inProgress &&((imageItem->getBufferQueueSize() == 0)))
      {
         Sleep(10);
      }
      while (imageItem->getBufferQueueSize() > 0)
      {
         bufferStart = imageItem->getNextBuffer(&validFrames);
         frameIterator = bufferStart;
         if (frameIterator != NULL)
         {
            for (unsigned long i = 0; i < validFrames; i++)
            {
               result = fp->process((uint16_t *)frameIterator);
// MUST USE RESULT IN FURTHER CALCULATIONS
               frameIterator += frameSize;
               processedFrameCount++;
               free(result);
            }
            writeFile(bufferStart, (validFrames * frameSize), filename);
            free(bufferStart);
         }

      }
   }
}

void ImageProcessor::processThresholdValue(GeneralFrameProcessor *fp, int thresholdValue, uint16_t *result, const char* filename)
{
   unsigned long validFrames = 0;
   char *bufferStart;
   char *frameIterator;

   filename = "C://karen//STFC//Technical//PLTest//re_orderThreshVal.bin";
   thresholdValue = processingDefinition->getThresholdValue();
   qDebug() << "ImageProcessor::process() ThresholdMode::SINGLE_VALUE" << thresholdValue;
   while (inProgress || (imageItem->getBufferQueueSize() > 0))
   {
      while (inProgress &&((imageItem->getBufferQueueSize() == 0)))
      {
         Sleep(10);
      }
      while (imageItem->getBufferQueueSize() > 0)
      {
         bufferStart = imageItem->getNextBuffer(&validFrames);
         frameIterator = bufferStart;
         if (frameIterator != NULL)
         {
            for (unsigned long i = 0; i < validFrames; i++)
            {
               result = fp->process((uint16_t *)frameIterator, thresholdValue);
               // MUST USE RESULT IN FURTHER CALCULATIONS
               frameIterator += frameSize;
               processedFrameCount++;
            }
            qDebug() << "Writing to file: " << (validFrames * frameSize);
            writeFile(bufferStart, (validFrames * frameSize), filename);
            free(bufferStart);
         }
      }
   }
}

void ImageProcessor::processThresholdFile(GeneralFrameProcessor *fp, uint16_t *thresholdPerPixel, uint16_t *result, const char* filename)
{
   unsigned long validFrames = 0;
   char *bufferStart;
   char *frameIterator;

   filename = "C://karen//STFC//Technical//PLTest//re_orderThreshPerPix.bin";
   qDebug() << "ImageProcessor::process() ThresholdMode::THRESHOLD_FILE";
   thresholdPerPixel = processingDefinition->getThresholdPerPixel();
   while (inProgress || (imageItem->getBufferQueueSize() > 0))
   {
      while (inProgress &&((imageItem->getBufferQueueSize() == 0)))
      {
         Sleep(10);
      }
      while (imageItem->getBufferQueueSize() > 0)
      {
         bufferStart = imageItem->getNextBuffer(&validFrames);
         frameIterator = bufferStart;
         if (frameIterator != NULL)
         {
            for (unsigned long i = 0; i < validFrames; i++)
            {
               result = fp->process((uint16_t *)frameIterator, thresholdPerPixel);
               // MUST USE RESULT IN FURTHER CALCULATIONS
               frameIterator += frameSize;
               processedFrameCount++;
            }
            writeFile(bufferStart, (validFrames * frameSize), filename);
            free(bufferStart);
         }
      }
   }
}

void ImageProcessor::handleProcess()
{
   GeneralFrameProcessor *fp;
   int thresholdValue;
   uint16_t *thresholdPerPixel;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshVal.bin";
   uint16_t *result;
   bool energyCalibration;

   processedFrameCount = 0;

   if (processingDefinition->getRe_order())
   {
      fp = new FrameRe_orderProcessor();
   }
   else
   {
      fp = new FrameProcessor();
   }

   if ((energyCalibration = processingDefinition->getEnergyCalibration()))
   {
      if (processingDefinition->getTotalSpectrum())
      {
         hxtGenerator = new HxtTotalSpectrumGenerator();
      }
      else
      {
         hxtGenerator = new HxtGenerator();
      }
      connect(fp->getPixelProcessor(), SIGNAL(enqueuePixelEnergy(double *)),
              hxtGenerator, SLOT(handleEnqueuePixelEnergy(double *)));
   }

   fp->setEnergyCalibration(energyCalibration);
   fp->setGradients(processingDefinition->getGradients());
   fp->setIntercepts(processingDefinition->getIntercepts());

   switch (processingDefinition->getThreshholdMode())
   {
      case ThresholdMode::NONE:
         processThresholdNone(fp, result, filename);
         break;
      case ThresholdMode::SINGLE_VALUE:
         processThresholdValue(fp, thresholdValue, result, filename);
         break;
      case ThresholdMode::THRESHOLD_FILE:
         processThresholdFile(fp, thresholdPerPixel, result, filename);
         break;
      default:
         break;
   }

   qDebug() << "ImageProcessor::process() processedFrameCount = " << processedFrameCount;
   emit processingComplete(this, processedFrameCount);
   free(imageItem);
   free(fp);
}

void ImageProcessor::enqueueBuffer(char *bufferToProcess, unsigned long validFrames)
{
   imageItem->enqueueBuffer(bufferToProcess, validFrames);
}

void ImageProcessor::setImageInProgress(bool inProgress)
{
   this->inProgress = inProgress;
}

void ImageProcessor::imageComplete(unsigned long long totalFramesToProcess)
{
   this->totalFramesToProcress = totalFramesToProcess;
   setImageInProgress(false);
}

void ImageProcessor::writeFile(char *buffer, unsigned long length, const char* filename)
{
   std::ofstream outFile;

   outFile.open(filename, std::ofstream::binary | std::ofstream::app);
   outFile.write((const char *)buffer, length * sizeof(char));
   outFile.close();

}
