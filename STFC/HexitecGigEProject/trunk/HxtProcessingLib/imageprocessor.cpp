#include "imageprocessor.h"
#include <QThread>
#include <QDebug>
#include <QTime>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>

ImageProcessor::ImageProcessor(const char *filename, int nRows, int nCols, ProcessingDefinition *processingDefinition)
{
   frameSize = nRows * nCols * sizeof(uint16_t);
   imageProcessorThread = new QThread();
   imageProcessorThread->start();
   moveToThread(imageProcessorThread);
   this->frameSize = frameSize;
   imageItem = new ImageItem(filename, nRows, nCols);
   this->processingDefinition = processingDefinition;
   energyCalibration = processingDefinition->getEnergyCalibration();
   nextFrameCorrection = processingDefinition->getNextFrameCorrection();
   if (processingDefinition->getChargedSharingMode() == OFF)
   {
      chargedSharing = false;
   }
   else
   {
      chargedSharing = true;
   }


   if (energyCalibration)
   {
      if (chargedSharing)
      {
         hxtGenerator = new HxtChargedSharingGenerator(nRows, nCols, processingDefinition);
      }
      else
      {
         hxtGenerator = new HxtGenerator(nRows, nCols, processingDefinition);
      }
   }

   setImageInProgress(true);
   connect(this, SIGNAL(process()), this, SLOT(handleProcess()));

   emit process();
}

ImageProcessor::~ImageProcessor()
{
   imageProcessorThread->exit();
}

void ImageProcessor::processThresholdNone(GeneralFrameProcessor *fp, uint16_t *result, const char* filenameBin, const char* filenameHxt, const char *filenameCsv)
{
   unsigned long validFrames = 0;
   char *bufferStart;
   char *frameIterator;
   int buffNo = 0;

   qDebug() << " ImageProcessor output files: " << filenameBin << filenameHxt;

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
                  result = fp->process((uint16_t *)frameIterator, &pixelEnergyMap);

                  if (pixelEnergyMap->size() > 0)
                  {
                     hxtGenerator->enqueuePixelEnergyMap(pixelEnergyMap);
                     processedFrameCount++;
                  }
                  // MUST USE RESULT IN FURTHER CALCULATIONS
                  frameIterator += frameSize;
                  free(result);
               }
               writeBinFile(bufferStart, (validFrames * frameSize), filenameBin);
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
               writeBinFile(bufferStart, (validFrames * frameSize), filenameBin);
               free(bufferStart);
            }
         }
      }
      if (processingDefinition->getTotalSpectrum())
      {
         writeCsvFile(hxtGenerator->getEnergyBin(), hxtGenerator->getSummedHistogram(), filenameCsv);
      }
   }
}

void ImageProcessor::processThresholdValue(GeneralFrameProcessor *fp, int thresholdValue, uint16_t *result,
                                           const char* filenameBin, const char *filenameHxt, const char *filenameCsv)
{
   unsigned long validFrames = 0;
   char *bufferStart;
   char *frameIterator;

   thresholdValue = processingDefinition->getThresholdValue();

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
               writeBinFile(bufferStart, (validFrames * frameSize), filenameBin);
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
               writeBinFile(bufferStart, (validFrames * frameSize), filenameBin);
               free(bufferStart);
            }
         }
      }
      if (processingDefinition->getTotalSpectrum())
      {
         writeCsvFile(hxtGenerator->getEnergyBin(), hxtGenerator->getSummedHistogram(), filenameCsv);
      }
   }
}


void ImageProcessor::processThresholdFile(GeneralFrameProcessor *fp, uint16_t *thresholdPerPixel, uint16_t *result,
                                          const char* filenameBin, const char *filenameHxt, const char *filenameCsv)
{
   unsigned long validFrames = 0;
   char *bufferStart;
   char *frameIterator;

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
               }
               writeBinFile(bufferStart, (validFrames * frameSize), filenameBin);
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
               writeBinFile(bufferStart, (validFrames * frameSize), filenameBin);
               free(bufferStart);
            }
         }
      }
      if (processingDefinition->getTotalSpectrum())
      {
         writeCsvFile(hxtGenerator->getEnergyBin(), hxtGenerator->getSummedHistogram(), filenameCsv);
      }
   }
}

void ImageProcessor::handleProcess()
{
   GeneralFrameProcessor *fp;
   int thresholdValue;
   uint16_t *thresholdPerPixel;
   uint16_t *result;
   bool energyCalibration;
   char* filenameBin = new char[1024];
   char* filenameHxt = new char[1024];
   char* filenameCsv = new char[1024];

   strcpy(filenameBin, imageItem->getFilename());
   strcpy(filenameHxt, filenameBin);
   strcpy(filenameCsv, filenameBin);
   strcat(filenameBin,".bin");
   strcat(filenameHxt,".hxt");
   strcat(filenameCsv,"_total.csv");

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
         processThresholdNone(fp, result, filenameBin, filenameHxt, filenameCsv);
         break;
      case ThresholdMode::SINGLE_VALUE:
         processThresholdValue(fp, thresholdValue, result, filenameBin, filenameHxt, filenameCsv);
         break;
      case ThresholdMode::THRESHOLD_FILE:
         processThresholdFile(fp, thresholdPerPixel, result, filenameBin, filenameHxt, filenameCsv);
         break;
      default:
         break;
   }

   qDebug() << "ImageProcessor::process() processedFrameCount = " << processedFrameCount << " waiting for energy processing";
   emit imageComplete(processedFrameCount);
   while (hxtGenerator->getProcessedEnergyCount() < processedFrameCount)
   {
      Sleep(10);
   }
   hxtGenerator->setFrameProcessingInProgress(false);
   qDebug() << "ImageProcessor::process() processedEnergyCount() = " << hxtGenerator->getProcessedEnergyCount()
            << " energy processing complete. File writen " << imageItem->getFilename();
   emit processingComplete(this, processedFrameCount);
   qDebug() << "++++++++++++++STOP TIMER";
   qDebug() << "PROCESSING TOOK: "<< time.elapsed() << " mSecs";

   delete imageItem;
   delete fp;
}

void ImageProcessor::enqueueBuffer(char *bufferToProcess, unsigned long validFrames)
{
   imageItem->enqueueBuffer(bufferToProcess, validFrames);
}

void ImageProcessor::setImageInProgress(bool inProgress)
{
   qDebug() << "Setting image inProgress: " << inProgress;
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
   unsigned long long theAddress;
   memcpy((void *) &theAddress, (void *) (header+184), 8);

   outFile.open(filename, std::ofstream::binary | std::ofstream::out);
   outFile.write((const char *)header, headerLength * sizeof(char));
   outFile.write((const char *)data, dataLength * sizeof(char));
   outFile.close();
   emit hxtFileWritten(header, filename);
//   Sleep(500);
}

void ImageProcessor::writeCsvFile(double *energyBin, long long *summedHistogram,  const char *filename)
{
   std::ofstream outFile;
   long long nBins = processingDefinition->getNBins();
   double *energyBinPtr;
   long long *summedHistogramPtr = summedHistogram;
   energyBinPtr = energyBin;

   qDebug() <<"ImageProcessor::writeCsvFile: " << filename;

   ostringstream csvOutput;
   for (int i = 0; i < nBins; i++)
   {
      csvOutput << *energyBinPtr << "," << *summedHistogramPtr << "\n";
      energyBinPtr++;
      summedHistogramPtr++;
   }

   std::string s = csvOutput.str();
   outFile.open(filename, std::ofstream::out);
   outFile << s;
   outFile.close();

}
