#include "imageprocessor.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
//
#include <QDebug>
#include <QTime>

HANDLE ImageProcessor::getProcessingCompleteEvent()
{
   return processingCompleteEvent;
}

HANDLE ImageProcessor::getImageCompleteEvent()
{
   return imageCompleteEvent;
}

HANDLE ImageProcessor::getHxtFileWrittenEvent()
{
   return hxtFileWrittenEvent;
}

ImageProcessor::ImageProcessor(const char *filename, int nRows, int nCols, ProcessingDefinition *processingDefinition)
{
   imageCompleteEvent = CreateEvent(NULL, FALSE, FALSE, IMAGE_COMPLETE);
   processingCompleteEvent = CreateEvent(NULL, FALSE, FALSE, PROCESSING_COMPLETE);
   hxtFileWrittenEvent = CreateEvent(NULL, FALSE, FALSE, HXT_FILE_WRITTEN);

   frameSize = nRows * nCols * sizeof(uint16_t);

   this->frameSize = frameSize;
   imageItem = new ImageItem(filename, nRows, nCols);
   this->processingDefinition = processingDefinition;
   energyCalibration = processingDefinition->getEnergyCalibration();
   hxtGeneration = processingDefinition->getHxtGeneration();
   nextFrameCorrection = processingDefinition->getNextFrameCorrection();
   totalSpectrum = processingDefinition->getTotalSpectrum();

   if (processingDefinition->getChargedSharingMode() == OFF)
   {
      chargedSharing = false;
   }
   else
   {
      chargedSharing = true;
   }
    qDebug() << "ImageProcessor() chargedSharing: " << chargedSharing << " totalSpectrum: " << totalSpectrum;
      if (chargedSharing)
      {
         if(totalSpectrum)
         {
            hxtGenerator = new HxtChargedSharingSumGenerator(processingDefinition->getRows(), processingDefinition->getCols(), processingDefinition);
         }
         else
         {
            hxtGenerator = new HxtChargedSharingGenerator(processingDefinition->getRows(), processingDefinition->getCols(), processingDefinition);
         }
      }
      else
      {
         if(totalSpectrum)
         {
            hxtGenerator = new HxtSumGenerator(processingDefinition->getRows(), processingDefinition->getCols(), processingDefinition);
         }
         else
         {
            hxtGenerator = new HxtGenerator(processingDefinition->getRows(), processingDefinition->getCols(), processingDefinition);
         }
      }

   setImageInProgress(true);
}

ImageProcessor::~ImageProcessor()
{
}

void ImageProcessor::processThresholdNone(GeneralFrameProcessor *fp, uint16_t *result, const char* filenameBin, const char* filenameHxt, const char *filenameCsv)
{
   unsigned long validFrames = 0;
   char *bufferStart;
   char *frameIterator;
   int buffNo = 0;
    qDebug() << Q_FUNC_INFO;

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
                  result = fp->process((uint16_t *)frameIterator, &hxtMap);

//                  if (hxtMap->size() > 0)
//                  {
                      qDebug() << "processThresholdNone()";
                      hxtGenerator->processEnergies((uint16_t *)result);
//                      hxtGenerator->calibrateAndApplyChargedAlgorithm((uint16_t *)frameIterator, thresholdValue,
//                                                                      processingDefinition->getGradients(), processingDefinition->getIntercepts());
//                  }
                  // MUST USE RESULT IN FURTHER CALCULATIONS
                  frameIterator += frameSize;
                  processedFrameCount++;
                  free(result);
               }
               writeBinFile(bufferStart, (validFrames * frameSize), filenameBin);
               if (hxtGeneration)
               {
                  writeHxtFile((char *) hxtGenerator->getHxtV3Buffer(), processingDefinition->getHxtBufferHeaderSize(),
                               (char *) hxtGenerator->getHxtV3AllData(), processingDefinition->getHxtBufferAllDataSize(),
                               filenameHxt);

               }
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
                  result = fp->process(&hxtMap, (uint16_t *)frameIterator);
                  // MUST USE RESULT IN FURTHER CALCULATIONS
                  frameIterator += frameSize;
                  processedFrameCount++;
                  free(result);
               }
               writeBinFile(bufferStart, (validFrames * frameSize), filenameBin);
               if (hxtGeneration)
               {
                  writeHxtFile((char *) hxtGenerator->getHxtV3Buffer(), processingDefinition->getHxtBufferHeaderSize(),
                               (char *) hxtGenerator->getHxtV3AllData(), processingDefinition->getHxtBufferAllDataSize(),
                               filenameHxt);

               }
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

   ///
   qDebug() << Q_FUNC_INFO;
//   QTime qtTime;
//   int accessTime = 0, processTime = 0, energiesTime = 0, binaryTime = 0, hxtTime = 0, spectrumTime = 0;
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
//            qtTime.restart();
            bufferStart = imageItem->getNextBuffer(&validFrames);
//            accessTime = qtTime.elapsed();
            frameIterator = bufferStart;
            if (frameIterator != NULL)
            {

               for (unsigned long i = 0; i < validFrames; i++)
               {
//                  qtTime.restart();
                  result = fp->process((uint16_t *)frameIterator, thresholdValue, &hxtMap);
                  qDebug() << "IP fp done";
//                  processTime = qtTime.elapsed();
                  ///if (hxtMap->size() > 0)
                  {
                     /// Do calibration & CS algorithm in one go
//                     qtTime.restart();
                     ///hxtGenerator->processEnergies(hxtMap);
                     qDebug() << "IP before hxtG->procNrg()..";
                     hxtGenerator->processEnergies((uint16_t *)result);
//                     energiesTime = qtTime.elapsed();
                  }
                  qDebug() << "IP processEnergies() done";
                  // MUST USE RESULT IN FURTHER CALCULATIONS
                  frameIterator += frameSize;
                  processedFrameCount++;
                  qDebug() << "IP before freeing results";
                  free(result);
                  qDebug() << "IP freed result";
               }
//               qtTime.restart();
               writeBinFile(bufferStart, (validFrames * frameSize), filenameBin);
//               binaryTime = qtTime.elapsed();
               if (hxtGeneration)
               {
//                  qtTime.restart();
                  writeHxtFile((char *) hxtGenerator->getHxtV3Buffer(), processingDefinition->getHxtBufferHeaderSize(),
                               (char *) hxtGenerator->getHxtV3AllData(), processingDefinition->getHxtBufferAllDataSize(),
                               filenameHxt);
//                 hxtTime = qtTime.elapsed();

               }
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
                  result = fp->process(&hxtMap, (uint16_t *)frameIterator, thresholdValue);
//                  hxtGenerator->processEnergies(hxtMap);
                  hxtGenerator->processEnergies((uint16_t *)result);
                  frameIterator += frameSize;
                  processedFrameCount++;
                  free(result);
               }
               writeBinFile(bufferStart, (validFrames * frameSize), filenameBin);
               free(bufferStart);
               if (hxtGeneration)
               {
                  writeHxtFile((char *) hxtGenerator->getHxtV3Buffer(), processingDefinition->getHxtBufferHeaderSize(),
                               (char *) hxtGenerator->getHxtV3AllData(), processingDefinition->getHxtBufferAllDataSize(),
                               filenameHxt);

               }
            }
         }
      }
      if (processingDefinition->getTotalSpectrum())
      {
//         qtTime.restart();
         writeCsvFile(hxtGenerator->getEnergyBin(), hxtGenerator->getSummedHistogram(), filenameCsv);
//         spectrumTime = qtTime.elapsed();
      }
   }
//   qDebug() << "IP    access: " << (accessTime) << " ms.";
//   qDebug() << "IP   process: " << (processTime) << " ms.";
//   qDebug() << "IP  energies: " << (energiesTime) << " ms.";
//   qDebug() << "IP  binWrite: " << (binaryTime) << " ms.";
//   qDebug() << "IP  hxtWrite: " << (hxtTime) << " ms.";
//   qDebug() << "IP  spectrum: " << (spectrumTime) << " ms.";
//   qDebug() << "IP current time: " << QTime::currentTime();
}


void ImageProcessor::processThresholdFile(GeneralFrameProcessor *fp, uint16_t *thresholdPerPixel, uint16_t *result,
                                          const char* filenameBin, const char *filenameHxt, const char *filenameCsv)
{
   unsigned long validFrames = 0;
   char *bufferStart;
   char *frameIterator;

   qDebug() << Q_FUNC_INFO;

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
                  result = fp->process((uint16_t *)frameIterator, thresholdPerPixel, &hxtMap);
//                  if (hxtMap->size() > 0)
//                  {
                      ///hxtGenerator->processEnergies(hxtMap);
                      hxtGenerator->processEnergies((uint16_t *)result);
//                      hxtGenerator->calibrateAndApplyChargedAlgorithm((uint16_t *)frameIterator, thresholdValue,
//                                                                      processingDefinition->getGradients(), processingDefinition->getIntercepts());
//                  }
                  // MUST USE RESULT IN FURTHER CALCULATIONS
                  frameIterator += frameSize;
                  processedFrameCount++;
                  free(result);
               }
               writeBinFile(bufferStart, (validFrames * frameSize), filenameBin);
               if (hxtGeneration)
               {
                  writeHxtFile((char *) hxtGenerator->getHxtV3Buffer(), processingDefinition->getHxtBufferHeaderSize(),
                               (char *) hxtGenerator->getHxtV3AllData(), processingDefinition->getHxtBufferAllDataSize(),
                               filenameHxt);

               }
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
                  result = fp->process(&hxtMap, (uint16_t *)frameIterator, thresholdPerPixel);
                  // MUST USE RESULT IN FURTHER CALCULATIONS
                  frameIterator += frameSize;
                  processedFrameCount++;
                  free(result);
               }
               writeBinFile(bufferStart, (validFrames * frameSize), filenameBin);
               if (hxtGeneration)
               {
                  writeHxtFile((char *) hxtGenerator->getHxtV3Buffer(), processingDefinition->getHxtBufferHeaderSize(),
                               (char *) hxtGenerator->getHxtV3AllData(), processingDefinition->getHxtBufferAllDataSize(),
                               filenameHxt);

               }
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
   int thresholdValue = 0;
   uint16_t *thresholdPerPixel = NULL;
   uint16_t *result = NULL;
   filenameBin = new char[1024];
   filenameHxt = new char[1024];
   filenameCsv = new char[1024];

   strcpy(filenameBin, imageItem->getFilename());
   strcpy(filenameHxt, filenameBin);
   strcpy(filenameCsv, filenameBin);
   strcat(filenameBin,".bin");
   strcat(filenameHxt,".hxt");
   strcat(filenameCsv,"_total.csv");

   processedFrameCount = 0;

   if (processingDefinition->getRe_order())
   {
      fp = new FrameRe_orderProcessor(nextFrameCorrection);
   }
   else
   {
      fp = new FrameProcessor(nextFrameCorrection);
   }

   fp->setFrameSize(processingDefinition->getFrameSize());
   fp->setEnergyCalibration(processingDefinition->getEnergyCalibration());
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


   hxtGenerator->setFrameProcessingInProgress(false);
   SetEvent(processingCompleteEvent);

   delete imageItem;
   delete fp;
   delete hxtGenerator;
}

void ImageProcessor::freeAllocedMemory()
{
   hxtGenerator->freeAllocedMemory();
}

char *ImageProcessor::getHxtFilename()
{
   return filenameHxt;
}

GeneralHxtGenerator *ImageProcessor::getHxtGenerator()
{
   return hxtGenerator;
}

void ImageProcessor::enqueueBuffer(char *bufferToProcess, unsigned long validFrames)
{
   imageItem->enqueueBuffer(bufferToProcess, validFrames);
}

void ImageProcessor::setImageInProgress(bool inProgress)
{
   this->inProgress = inProgress;
}

void ImageProcessor::imageAcquisitionComplete(long long totalFramesToProcess)
{
   this->totalFramesToProcess = totalFramesToProcess;
   setImageInProgress(false);
}

void ImageProcessor::writeBinFile(char *buffer, unsigned long length, const char* filename)
{
    qDebug() <<  "image processor::writingBinFile()";
   std::ofstream outFile;

   outFile.open(filename, std::ofstream::binary | std::ofstream::app);
   outFile.write((const char *)buffer, length * sizeof(char));
   outFile.close();   
   qDebug() <<  "image processor::writingBinFile() finished";
}

void ImageProcessor::writeHxtFile(char *header, unsigned long headerLength, char *data, unsigned long dataLength, const char *filename)
{
   std::ofstream outFile;

   outFile.open(filename, std::ofstream::binary | std::ofstream::out);
   outFile.write((const char *)header, headerLength * sizeof(char));
   outFile.write((const char *)data, dataLength * sizeof(char));
   outFile.close();
   SetEvent(hxtFileWrittenEvent);
}

void ImageProcessor::writeCsvFile(double *energyBin, long long *summedHistogram,  const char *filename)
{
   std::ofstream outFile;
   long long nBins = processingDefinition->getNBins();
   double *energyBinPtr;
   long long *summedHistogramPtr = summedHistogram;
   energyBinPtr = energyBin;

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
