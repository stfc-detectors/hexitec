#include "imageprocessor.h"
#include "frameprocessor.h"
#include <QThread>
#include <QDebug>
#include <Windows.h>

ImageProcessor::ImageProcessor(const char*name, int frameSize, ProcessingDefinition *processingDefinition)
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

void ImageProcessor::handleProcess()
{
   unsigned long validFrames = 0;
   FrameProcessor fp;
   char *bufferStart;
   char *frameIterator;
   int thresholdValue;
   uint16_t *thresholdPerPixel;

   processedFrameCount = 0;
   switch (processingDefinition->getThreshholdMode())
   {
      case ThresholdMode::NONE:
         qDebug() << "ImageProcessor::process() ThresholdMode::NONE";
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
                     fp.process((uint16_t *)frameIterator);
                     frameIterator += frameSize;
                     processedFrameCount++;
                  }
//                  free(bufferStart);
               }

            }
         }
         break;
      case ThresholdMode::SINGLE_VALUE:
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
               frameIterator = imageItem->getNextBuffer(&validFrames);
               if (frameIterator != NULL)
               {
                  for (unsigned long i = 0; i < validFrames; i++)
                  {
                     fp.process((uint16_t *)frameIterator, thresholdValue);
                     frameIterator += frameSize;
                     processedFrameCount++;
                  }
//                  free(bufferStart);
               }
            }
         }
         break;
      case ThresholdMode::THRESHOLD_FILE:
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
               frameIterator = imageItem->getNextBuffer(&validFrames);
               if (frameIterator != NULL)
               {
                  for (unsigned long i = 0; i < validFrames; i++)
                  {
                     fp.process((uint16_t *)frameIterator, thresholdPerPixel);
                     frameIterator += frameSize;
                     processedFrameCount++;
                  }
//                  free(bufferStart);

               }
            }
         }
         break;
      default:
         break;
   }
   qDebug() << "ImageProcessor::process() processedFrameCount = " << processedFrameCount;
   emit processingComplete(this, processedFrameCount);
   this->disconnect(SIGNAL(process));

}

void ImageProcessor::enqueueBuffer(char *bufferToProcess, unsigned long validFrames)
{
   imageItem->enqueueBuffer(bufferToProcess, validFrames);
   emit returnBufferReady(bufferToProcess);
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
