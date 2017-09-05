#include "processingbuffergenerator.h"
#include <QMutexLocker>
#include <QFileInfo>
#include <QDebug>
#include <QThread>
#include <iostream>
#include <fstream>
#include <Windows.h>

ProcessingBufferGenerator::ProcessingBufferGenerator(ProcessingDefinition *processingDefinition, QObject *parent) : QObject(parent)
{
   imageProcessorList.clear();
   currentImageProcessor = NULL;
   this->processingDefinition = processingDefinition;

   connect(this, SIGNAL(imageStarted(const char*, int, int)),
           this, SLOT(handleImageStarted(const char *, int, int)));
   connect(this, SIGNAL(transferBufferReady(char*, unsigned long)),
           this, SLOT(handleTransferBufferReady(char *, unsigned long)));
   connect(this, SIGNAL(imageComplete(long long)),
           this, SLOT(handleImageComplete(long long)));
}

void ProcessingBufferGenerator::enqueueImage(const char *filename, int nRows, int nCols, ProcessingDefinition *processingDefinition)
{
   this->frameSize = nRows * nCols * sizeof(uint16_t);
   currentImageProcessor = new ImageProcessor(filename, nRows, nCols, processingDefinition);
   connect(currentImageProcessor, SIGNAL(processingComplete(ImageProcessor *, long long)),
           this, SLOT(handleProcessingComplete(ImageProcessor *, long long)));
   imageProcessorList.append(currentImageProcessor);
   qDebug() << "IMAGE QUEUED: currentImageProcessor " << currentImageProcessor
            << "image queue length = " << imageProcessorList.length();
}

void ProcessingBufferGenerator::handleImageStarted(const char *filename, int nRows, int nCols)
{
   QMutexLocker locker(&mutex);
   enqueueImage(filename, nRows, nCols, processingDefinition);
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

void ProcessingBufferGenerator::handleConfigureProcessing(bool re_order, bool nextFrame,
                                                          int threshholdMode, int thresholdValue, const char *thresholdFilname)
{
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
}

void ProcessingBufferGenerator::handleConfigureProcessing(bool energyCalibration, long long binStart, long long binEnd, double binWidth, bool totalSpectrum,
                                                          char *gradientFilename,
                                                          char *interceptFilename,
                                                          const char *processedFilename)
{

   processingDefinition->setEnergyCalibration(energyCalibration);
   processingDefinition->setBinStart(binStart);
   processingDefinition->setBinEnd(binEnd);
   processingDefinition->setBinWidth(binWidth);
   processingDefinition->setTotalSpectrum(totalSpectrum);
   processingDefinition->setGradientFilename(gradientFilename);
   processingDefinition->setInterceptFilename(interceptFilename);
}

void ProcessingBufferGenerator::handleConfigureProcessing(int chargedSharingMode, int pixelGridOption)
{
   processingDefinition->setChargedSharingMode((ChargedSharingMode)chargedSharingMode);
   processingDefinition->setPixelGridSize(pixelGridOption);
}

void ProcessingBufferGenerator::handleProcessingComplete(ImageProcessor *completedImageProcessor, long long processedFrameCount)
{
   Sleep(500);
   imageProcessorList.removeOne(completedImageProcessor);
   delete completedImageProcessor;
}

void ProcessingBufferGenerator::handleConfigureProcessing(QStringList inputFilesList,
                                                          QString outputDirectory, QString outputPrefix)
{
   this->inputFilesList = inputFilesList;
   processingDefinition->setOutputDirectory((char *)outputDirectory.toStdString().c_str());
   processingDefinition->setOutputPrefix((char *)outputPrefix.toStdString().c_str());

}

void ProcessingBufferGenerator::handleProcessImages()
{
   int fileExtensionPos;
   QString outputFilename;
   char *transferBuffer;
   unsigned long validFrames = 0;
   long long totalFramesAcquired = 0;
   std::ifstream inFile;
   int nRows = 80;
   int nCols = 80;
   char *processingFilename;
   char *inputFilename;

   processingFilenameList.clear();

   inputFilename = new char[1024];

   foreach (const QString &str, inputFilesList)
   {
      QFileInfo fi(str);
      QString filename = fi.fileName();

      processingFilename = new char[1024];

      fileExtensionPos = filename.lastIndexOf(".");
      filename.truncate(fileExtensionPos);

      outputFilename = processingDefinition->getOutputDirectory();
      if (!outputFilename.endsWith("/"))
      {
         outputFilename.append("/");
      }
      outputFilename.append(processingDefinition->getOutputPrefix());
      outputFilename.append(filename);

      processingFilename = (char *)outputFilename.toStdString().c_str();
      inputFilename = (char *)str.toStdString().c_str();
      qDebug() << " ProcessingBufferGenerator::handleProcessImages() in foreach loop output name:"
               << outputFilename;

      inFile.open(inputFilename, ifstream::binary);
      processingFilenameList.append(processingFilename);

      emit imageStarted((const char *)processingFilenameList.back(), nRows, nCols);

      int bufferCount = 0;

      while (inFile)
      {
         transferBuffer = (char *) calloc(6400 * 500 * sizeof(uint16_t), sizeof(char));

         inFile.read(transferBuffer, 6400 * 500 * 2);
         if (!inFile)
         {
            validFrames = inFile.gcount() / (6400 * 2);
            emit transferBufferReady(transferBuffer, validFrames);
         }
         else
         {
            validFrames = inFile.gcount() / (6400 * 2);
            emit transferBufferReady(transferBuffer, 500);
         }
         totalFramesAcquired += validFrames;
         bufferCount++;
         Sleep(50);
      }
      inFile.close();
      emit imageComplete(totalFramesAcquired);
   }
   //   delete processingFilename;
   //   delete inputFilename;
}
