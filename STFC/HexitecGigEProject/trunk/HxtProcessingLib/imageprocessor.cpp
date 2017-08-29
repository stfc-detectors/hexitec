#include "imageprocessor.h"
#include <QThread>
#include <QDebug>
#include <QTime>
#include <Windows.h>
#include <iostream>
#include <fstream>

ImageProcessor::ImageProcessor(const char *name, int nRows, int nCols, ProcessingDefinition *processingDefinition)
{
   frameSize = nRows * nCols * sizeof(uint16_t);
   imageProcessorThread = new QThread();
   imageProcessorThread->start();
   moveToThread(imageProcessorThread);
   this->frameSize = frameSize;
   imageItem = new ImageItem(name, nRows, nCols);
   this->processingDefinition = processingDefinition;
   energyCalibration = processingDefinition->getEnergyCalibration();
   nextFrameCorrection = processingDefinition->getNextFrameCorrection();
   chargedSharingMode = processingDefinition->getChargedSharingMode();

   qDebug() << "ImageProcessor with energyCalibration = " << energyCalibration;
   qDebug() << "ImageProcessor with nextFrame = " << nextFrameCorrection;
   qDebug() << "ImageProcessor with chargedSharingMode = " << chargedSharingMode;

   if (energyCalibration)
   {
      if (processingDefinition->getTotalSpectrum())
      {
         hxtGenerator = new HxtTotalSpectrumGenerator(nRows, nCols, processingDefinition);
      }
      else
      {
//         hxtGenerator = new HxtGenerator(nRows, nCols, processingDefinition);
         hxtGenerator = new HxtChargedSharingGenerator(nRows, nCols, processingDefinition);
      }
   }

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
   const char* filenameHxt;
   int buffNo = 0;

   filename = "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//re_order.bin";
   filenameHxt = "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//re_order.hxt";
   qDebug() << "ImageProcessor::process() ThresholdMode::NONE";
   while (inProgress || (imageItem->getBufferQueueSize() > 0))
   {
      while (inProgress &&((imageItem->getBufferQueueSize() == 0)))
      {
         Sleep(10);
      }
      if (energyCalibration)
      {
         while (imageItem->getBufferQueueSize() > 0)
         {
            bufferStart = imageItem->getNextBuffer(&validFrames);
            frameIterator = bufferStart;
            qDebug() << "11111111";
            if (frameIterator != NULL)
            {
               for (unsigned long i = 0; i < validFrames; i++)
               {
                  result = fp->process((uint16_t *)frameIterator, &pixelEnergyMap);
                  if (pixelEnergyMap->size() > 0)
                  {
                     hxtGenerator->enqueuePixelEnergyMap(pixelEnergyMap);
                     processedFrameCount++;
                  }
                  // MUST USE RESULT IN FURTHER CALCULATIONS
                  frameIterator += frameSize;
//                  processedFrameCount++;
                  free(result);
               }
               writeBinFile(bufferStart, (validFrames * frameSize), filename);
               qDebug() << "writing buffer" << buffNo << " to binary file: validFrames: " << validFrames << " frameSize: " << frameSize;
               qDebug() << processingDefinition->getHxtBufferHeaderSize()
                        <<  " : " << processingDefinition->getHxtBufferAllDataSize()<<  " : " << filenameHxt;

               writeHxtFile((char *) hxtGenerator->getHxtV3Buffer(), processingDefinition->getHxtBufferHeaderSize(),
                            (char *) hxtGenerator->getHxtV3AllData(), processingDefinition->getHxtBufferAllDataSize(),
                            filenameHxt);
               free(bufferStart);
               buffNo++;
            }
         }
      }
      else
      {
         qDebug() << "NO Energy Calibration";
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
               writeBinFile(bufferStart, (validFrames * frameSize), filename);
               free(bufferStart);
            }
         }

      }
   }
}

void ImageProcessor::processThresholdValue(GeneralFrameProcessor *fp, int thresholdValue, uint16_t *result, const char* filename)
{
   unsigned long validFrames = 0;
   char *bufferStart;
   char *frameIterator;
   const char* filenameHxt;

   filename = "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//re_orderThreshVal.bin";
   filenameHxt = "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//re_orderThreshVal.hxt";
   thresholdValue = processingDefinition->getThresholdValue();
   qDebug() << "ImageProcessor::process() ThresholdMode::SINGLE_VALUE" << thresholdValue;
   while (inProgress || (imageItem->getBufferQueueSize() > 0))
   {
      while (inProgress &&((imageItem->getBufferQueueSize() == 0)))
      {
         Sleep(10);
      }
      if (energyCalibration)
      {
         while (imageItem->getBufferQueueSize() > 0)
         {
            bufferStart = imageItem->getNextBuffer(&validFrames);
            frameIterator = bufferStart;
            if (frameIterator != NULL)
            {
               for (unsigned long i = 0; i < validFrames; i++)
               {
                  result = fp->process((uint16_t *)frameIterator, thresholdValue, &pixelEnergyMap);
                  if (pixelEnergyMap->size() > 0)
                  {
                     hxtGenerator->enqueuePixelEnergyMap(pixelEnergyMap);
                     processedFrameCount++;
                  }
                  // MUST USE RESULT IN FURTHER CALCULATIONS
                  frameIterator += frameSize;
//                  processedFrameCount++;
                  free(result);
               }
               writeBinFile(bufferStart, (validFrames * frameSize), filename);
               writeHxtFile((char *) hxtGenerator->getHxtV3Buffer(), processingDefinition->getHxtBufferHeaderSize(),
                            (char *) hxtGenerator->getHxtV3AllData(), processingDefinition->getHxtBufferAllDataSize(),
                            filenameHxt);
               free(bufferStart);
            }
         }
      }
      else
      {
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
                  free(result);
               }
               writeBinFile(bufferStart, (validFrames * frameSize), filename);
               free(bufferStart);
            }
         }
      }
   }
}


void ImageProcessor::processThresholdFile(GeneralFrameProcessor *fp, uint16_t *thresholdPerPixel, uint16_t *result, const char* filename)
{
   unsigned long validFrames = 0;
   char *bufferStart;
   char *frameIterator;
   const char* filenameHxt;

   filename = "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//re_orderThreshPerPix.bin";
   filenameHxt = "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//re_orderThreshPerPix.hxt";
   qDebug() << "ImageProcessor::process() ThresholdMode::THRESHOLD_FILE";
   thresholdPerPixel = processingDefinition->getThresholdPerPixel();
   while (inProgress || (imageItem->getBufferQueueSize() > 0))
   {
      while (inProgress &&((imageItem->getBufferQueueSize() == 0)))
      {
         Sleep(10);
      }
      if (energyCalibration)
      {
         while (imageItem->getBufferQueueSize() > 0)
         {
            bufferStart = imageItem->getNextBuffer(&validFrames);
            frameIterator = bufferStart;
            if (frameIterator != NULL)
            {
               for (unsigned long i = 0; i < validFrames; i++)
               {
                  result = fp->process((uint16_t *)frameIterator, thresholdPerPixel, &pixelEnergyMap);
                  if (pixelEnergyMap->size() > 0)
                  {
                     hxtGenerator->enqueuePixelEnergyMap(pixelEnergyMap);
                     processedFrameCount++;
                  }
                  // MUST USE RESULT IN FURTHER CALCULATIONS
                  frameIterator += frameSize;
                  qDebug() << "done a frame";
               }
               writeBinFile(bufferStart, (validFrames * frameSize), filename);
               writeHxtFile((char *) hxtGenerator->getHxtV3Buffer(), processingDefinition->getHxtBufferHeaderSize(),
                            (char *) hxtGenerator->getHxtV3AllData(), processingDefinition->getHxtBufferAllDataSize(),
                            filenameHxt);
               free(bufferStart);
            }
         }
      }
      else
      {
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
                  free(result);
               }
               writeBinFile(bufferStart, (validFrames * frameSize), filename);
               free(bufferStart);
            }
         }
      }
   }
}

void ImageProcessor::handleProcess()
{
   GeneralFrameProcessor *fp;
   int thresholdValue;
   uint16_t *thresholdPerPixel;
   const char* filename = "C://karen//STFC//Technical//DSoFt_NewProcessingLib_Images//re_orderThreshVal.bin";
   uint16_t *result;
   bool energyCalibration;

   qDebug() << "ImageProcessor::handleProcess(), nextFrameCorrection: " << nextFrameCorrection;
   processedFrameCount = 0;
   qDebug() << "++++++++++++++START TIMER";
   QTime time;
   time.start();
   // operation

   if (processingDefinition->getRe_order())
   {
      fp = new FrameRe_orderProcessor(nextFrameCorrection);
   }
   else
   {
      fp = new FrameProcessor(nextFrameCorrection);
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

   qDebug() << "ImageProcessor::process() processedFrameCount = " << processedFrameCount << " waiting for energy processing";
   emit imageComplete(processedFrameCount);
   while (hxtGenerator->getProcessedEnergyCount() < processedFrameCount)
   {
      qDebug() << "ImageProcessor::process() processedEnergyCount() = " << hxtGenerator->getProcessedEnergyCount() << " energy processing on-going";
      Sleep(50);
   }
   hxtGenerator->setFrameProcessingInProgress(false);
   qDebug() << "ImageProcessor::process() processedEnergyCount() = " << hxtGenerator->getProcessedEnergyCount() << " energy processing complete";
   emit processingComplete(this, processedFrameCount);
   qDebug() << "++++++++++++++STOP TIMER";
   qDebug() << "PROCESSING TOOK: "<< time.elapsed() << " mSecs";

   free(imageItem);
   free(fp);
}

void ImageProcessor::enqueueBuffer(char *bufferToProcess, unsigned long validFrames)
{
   imageItem->enqueueBuffer(bufferToProcess, validFrames);
}

void ImageProcessor::setImageInProgress(bool inProgress)
{
//   hxtGenerator->setImageInProgress(inProgress);
   this->inProgress = inProgress;
}

void ImageProcessor::imageAcquisitionComplete(long long totalFramesToProcess)
{
   qDebug() << "ImageProcessor::imageComplete() totalFramesToProcess = " << totalFramesToProcess;
   this->totalFramesToProcess = totalFramesToProcess;
   setImageInProgress(false);
}

void ImageProcessor::writeBinFile(char *buffer, unsigned long length, const char* filename)
{
   std::ofstream outFile;

   outFile.open(filename, std::ofstream::binary | std::ofstream::app);
   outFile.write((const char *)buffer, length * sizeof(char));
   outFile.close();   
}

void ImageProcessor::writeHxtFile(char *header, unsigned long headerLength, char *data, unsigned long dataLength, const char *filename)
{
   std::ofstream outFile;

   outFile.open(filename, std::ofstream::binary | std::ofstream::out);
   outFile.write((const char *)header, headerLength * sizeof(char));
   outFile.write((const char *)data, dataLength * sizeof(char));
   outFile.close();
}
