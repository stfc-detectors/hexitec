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
   char *frameIterator;
   int thresholdValue;
   uint16_t *thresholdPerPixel;

   processedFrameCount = 0;
   qDebug() << "ImageProcessor current thread = " << QThread::currentThreadId();
   switch (processingDefinition->getThreshholdMode())
   {
      case ThresholdMode::NONE:
         qDebug() << "ImageProcessor::process() ThresholdMode::NONE";
         while (inProgress || (imageItem->getBufferQueueSize() > 0))
         {
            qDebug() << "ImageProcessor::process() BufferQueueSize() = " << imageItem->getBufferQueueSize();
            while (inProgress &&((frameIterator = imageItem->getNextBuffer(&validFrames)) == NULL))
            {
               //qDebug() << "ImageProcessor::process() ThresholdMode::NONE 22222";
               Sleep(0.1);
            }
            qDebug() << "ImageProcessor::process() ThresholdMode::NONE 33333";
            if (frameIterator != NULL)
            {
               qDebug() << "*******************validFrames = " << validFrames;
               for (unsigned long i = 0; i < validFrames; i++)
               {
                  fp.process((uint16_t *)frameIterator);
                  frameIterator+=frameSize;
                  processedFrameCount++;
               }
            }
         }
         break;
      case ThresholdMode::SINGLE_VALUE:
         qDebug() << "ImageProcessor::process() ThresholdMode::SINGLE_VALUE";
         thresholdValue = processingDefinition->getThresholdValue();
         while (inProgress || (imageItem->getBufferQueueSize() > 0))
         {
            while ((frameIterator = imageItem->getNextBuffer(&validFrames)) != NULL)
            {
               qDebug() << "*******************validFrames = " << validFrames;
               for (unsigned long i = 0; i < validFrames; i++)
               {
                  fp.process((uint16_t *)frameIterator, thresholdValue);
                  frameIterator+=frameSize;
                  processedFrameCount++;
               }
            }
         }
         break;
      case ThresholdMode::THRESHOLD_FILE:
         qDebug() << "ImageProcessor::process() ThresholdMode::THRESHOLD_FILE";
         thresholdPerPixel = processingDefinition->getThresholdPerPixel();
         while (inProgress)
         {
            while ((frameIterator = imageItem->getNextBuffer(&validFrames)) != NULL)
            {
               for (unsigned long i = 0; i < validFrames; i++)
               {
                  fp.process((uint16_t *)frameIterator, thresholdPerPixel);
                  frameIterator+=frameSize;
                  processedFrameCount++;
               }
            }
         }
         break;
      default:
         break;
   }
   qDebug() << "ImageProcessor::process() processedFrameCount = " << processedFrameCount;

}

void ImageProcessor::handleEnqueueBuffer(char *bufferToProcess, unsigned long validFrames)
{
   qDebug() << "ImageProcessor::enqueue thread = " << QThread::currentThreadId();
   imageItem->enqueueBuffer(bufferToProcess, validFrames);
}

void ImageProcessor::setImageInProgress(bool inProgress)
{
   this->inProgress = inProgress;
}

void ImageProcessor::handleImageComplete(unsigned long long totalFramesToProcess)
{
   this->totalFramesToProcress = totalFramesToProcess;
   setImageInProgress(false);
}
