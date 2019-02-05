#include "imageprocessor.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
//
#include <QDebug>
#include <QTime>
#include <QThread>

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
   imageCompleteEvent = CreateEvent(nullptr, FALSE, FALSE, IMAGE_COMPLETE);
   processingCompleteEvent = CreateEvent(nullptr, FALSE, FALSE, PROCESSING_COMPLETE);
   hxtFileWrittenEvent = CreateEvent(nullptr, FALSE, FALSE, HXT_FILE_WRITTEN);

   /// Assume library not built against GUI, unless HexitecGigE/ProcessingBufferGenerator emits mainWindowBusy(bool) signal
   bMainWindowBusy = false;

   frameSize = nRows * nCols * sizeof(uint16_t);

   this->frameSize = frameSize;
   imageItem = new ImageItem(filename);
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

   if (chargedSharing)
   {
      if (totalSpectrum)
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
      if (totalSpectrum)
      {
         hxtGenerator = new HxtSumGenerator(processingDefinition->getRows(), processingDefinition->getCols(), processingDefinition);
      }
      else
      {
         hxtGenerator = new HxtGenerator(processingDefinition->getRows(), processingDefinition->getCols(), processingDefinition);
      }
   }

   setImageInProgress(true);
   saveRaw = true;
   ///
}

ImageProcessor::~ImageProcessor()
{
   delete filenameBin;
   delete filenameHxt;
   delete filenameCsv;
}

void ImageProcessor::processThresholdNone(GeneralFrameProcessor *fp, double *result, const char* filenameBin, const char* filenameHxt, const char *filenameCsv)
{
   unsigned long validFrames = 0;
   char *bufferStart;
   char *frameIterator;
   unsigned int eventsInFrame = 0;
   unsigned long bufferEvents = 0;

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
            if (frameIterator != nullptr)
            {
               for (unsigned long i = 0; i < validFrames; i++)
               {
                  result = fp->process((uint16_t *)frameIterator, &hxtMap, &eventsInFrame);
                  hxtGenerator->processEnergies(result);
                  frameIterator += frameSize;
                  processedFrameCount++;
                  free(result);
                  bufferEvents += eventsInFrame;
                  eventsInFrame = 0;
               }
//               qDebug() << "\t" << i << "\tvalidFrames:" << validFrames << " bufferEvents:" << bufferEvents << "Average events/frame:" << (bufferEvents/i) << " buffEv/valFrms: " << (bufferEvents/validFrames);
               if (eventsMutex.tryLock(100))
               {
                  runningAverageEvents = (bufferEvents/validFrames);
                  eventsMutex.unlock();
               }
               bufferEvents = 0;
               if (saveRaw)
                  writeBinFile((char*)bufferStart, (validFrames * frameSize), filenameBin);
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
            if (frameIterator != nullptr)
            {
               for (unsigned long i = 0; i < validFrames; i++)
               {
                  result = fp->process(&hxtMap, (uint16_t *)frameIterator, &eventsInFrame);
                  hxtGenerator->processEnergies(result);
                  frameIterator += frameSize;
                  processedFrameCount++;
                  free(result);
                  bufferEvents += eventsInFrame;
                  eventsInFrame = 0;
               }
               if (eventsMutex.tryLock(100))
               {
                  runningAverageEvents = (bufferEvents/validFrames);
                  eventsMutex.unlock();
               }
               bufferEvents = 0;
               if (saveRaw)
                  writeBinFile((char*)bufferStart, (validFrames * frameSize), filenameBin);
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

unsigned long ImageProcessor::getRunningAverageEvents()
{
   unsigned long runningAverageEventsCopy = -1;
   if (eventsMutex.tryLock(100))
   {
      runningAverageEventsCopy = runningAverageEvents;
      eventsMutex.unlock();
   }
   return runningAverageEventsCopy;
}

void ImageProcessor::processThresholdValue(GeneralFrameProcessor *fp, int thresholdValue, double *result,
                                           const char* filenameBin, const char *filenameHxt, const char *filenameCsv)
{
   unsigned long validFrames = 0;
   char *bufferStart;
   char *frameIterator;
   unsigned int eventsInFrame = 0;
   unsigned long bufferEvents = 0;
//   QTime qtTime = QTime();
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
//            qtTime.restart();
            bufferStart = imageItem->getNextBuffer(&validFrames);
//            accessTime = qtTime.elapsed();
            frameIterator = bufferStart;
            if (frameIterator != nullptr)
            {
               for (unsigned long i = 0; i < validFrames; i++)
               {
                  result = fp->process((uint16_t *)frameIterator, thresholdValue, &hxtMap, &eventsInFrame);
                  hxtGenerator->processEnergies(result);
                  frameIterator += frameSize;
                  processedFrameCount++;
                  free(result);
                  bufferEvents += eventsInFrame;
                  eventsInFrame = 0;
               }
               if (eventsMutex.tryLock(100))
               {
                  runningAverageEvents = (bufferEvents/validFrames);
                  eventsMutex.unlock();
               }
               bufferEvents = 0;
//               qtTime.restart();
               if (saveRaw)
                  writeBinFile((char*)bufferStart, (validFrames * frameSize), filenameBin);
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
            if (frameIterator != nullptr)
            {
               for (unsigned long i = 0; i < validFrames; i++)
               {
                  result = fp->process(&hxtMap, (uint16_t *)frameIterator, thresholdValue, &eventsInFrame);
                  hxtGenerator->processEnergies(result);
                  frameIterator += frameSize;
                  processedFrameCount++;
                  free(result);
                  bufferEvents += eventsInFrame;
                  eventsInFrame = 0;
               }
               if (eventsMutex.tryLock(100))
               {
                  runningAverageEvents = (bufferEvents/validFrames);
                  eventsMutex.unlock();
               }
               bufferEvents = 0;
               if (saveRaw)
                  writeBinFile((char*)bufferStart, (validFrames * frameSize), filenameBin);
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


void ImageProcessor::processThresholdFile(GeneralFrameProcessor *fp, uint16_t *thresholdPerPixel, double *result,
                                          const char* filenameBin, const char *filenameHxt, const char *filenameCsv)
{
   unsigned long validFrames = 0;
   char *bufferStart;
   char *frameIterator;
   unsigned int eventsInFrame = 0;
   unsigned long bufferEvents = 0;

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
            bufferEvents = 0;
            frameIterator = bufferStart;
            if (frameIterator != nullptr)
            {
               for (unsigned long i = 0; i < validFrames; i++)
               {
                  result = fp->process((uint16_t *)frameIterator, thresholdPerPixel, &hxtMap, &eventsInFrame);
                  hxtGenerator->processEnergies(result);
                  frameIterator += frameSize;
                  processedFrameCount++;
                  free(result);
                  bufferEvents += eventsInFrame;
                  eventsInFrame = 0;
               }
               if (eventsMutex.tryLock(100))
               {
                  runningAverageEvents = (bufferEvents/validFrames);
                  eventsMutex.unlock();
               }
               bufferEvents = 0;
               if (saveRaw)
                  writeBinFile((char*)bufferStart, (validFrames * frameSize), filenameBin);
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
            if (frameIterator != nullptr)
            {
               for (unsigned long i = 0; i < validFrames; i++)
               {
                  result = fp->process(&hxtMap, (uint16_t *)frameIterator, thresholdPerPixel, &eventsInFrame);
                  hxtGenerator->processEnergies(result);
                  frameIterator += frameSize;
                  processedFrameCount++;
                  free(result);
                  bufferEvents += eventsInFrame;
                  eventsInFrame = 0;
               }
               if (eventsMutex.tryLock(100))
               {
                  runningAverageEvents = (bufferEvents/validFrames);
                  eventsMutex.unlock();
               }
               bufferEvents = 0;
               if (saveRaw)
                  writeBinFile((char*)bufferStart, (validFrames * frameSize), filenameBin);
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
   uint16_t *thresholdPerPixel = nullptr;
   double *result = nullptr;

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

   int occupancyThreshold = processingDefinition->getOccupancyThreshold();
   if (processingDefinition->getRe_order())
   {
      fp = new FrameRe_orderProcessor(nextFrameCorrection, occupancyThreshold);
   }
   else
   {
      fp = new FrameProcessor(nextFrameCorrection, occupancyThreshold);
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

   int corrections = fp->getOccupancyCorrections();
   qDebug() << "IP reports that an occupancyThreshold of" << occupancyThreshold << "produced" << corrections << "occupancy correction(s).";
   if (corrections != 80)
      emit occupancyCorrections(occupancyThreshold, corrections);

   hxtGenerator->setFrameProcessingInProgress(false);
   /*qDebug() << this << " IP::handleProcess()  - Wait if MainWindow busy..";*/
   bool bBusy = false;
   do
   {
      if (waitMutex.tryLock(100))
      {
         bBusy = bMainWindowBusy;
         waitMutex.unlock();
      }
      else
         qDebug() << this << " *** 1 Couldn't acquire waitMutex!";

      QThread::msleep(50);
   }
   while(bBusy);

   /*qDebug() << this << " IP::handleProcess()  - MainWindow ready, send final buffer";*/

   SetEvent(processingCompleteEvent);
   QThread::msleep(50);

   /*qDebug() << this << " IP::handleProcess()  - Wait for MainWindow do final buffer..";*/
   bBusy = false;
   do
   {
      if (waitMutex.tryLock(100))
      {
         bBusy = bMainWindowBusy;
         waitMutex.unlock();
      }
      else
         qDebug() << this << " *** 2 Couldn't acquire waitMutex!";

      QThread::msleep(50);
   }
   while(bBusy);

   delete imageItem;
   delete fp;
   //// Memory Leak versus calibration crash: If hxtGenerator is deleted, test.exe / HexitecGigE GUI
   ///   will crash if Calibration selected with bins: 0 / 200 / 0.25
   ///   (No crash with bins: 0 / 8000 / 10; nor without calibration selected)
//   delete hxtGenerator;	/// Memory leak; Freeing this memory would crash for most Calibration selection(s)
//   hxtGenerator = nullptr;

   imageItem = nullptr;
}

void ImageProcessor::handleMainWindowBusy(bool bBusy)
{

   if (waitMutex.tryLock(100))
   {
      this->bMainWindowBusy = bBusy;
      waitMutex.unlock();
   }

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

void ImageProcessor::setSaveRaw(bool bSaveRaw)
{
   saveRaw = bSaveRaw;
}
