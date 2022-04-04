#include "processingbuffergenerator.h"
#include "parameters.h"

#include <QSettings>
#include <QMutexLocker>
#include <QFileInfo>
#include <QDebug>
#include <QThread>
#include <iostream>
#include <fstream>
#include <Windows.h>

ProcessingBufferGenerator::ProcessingBufferGenerator(ProcessingDefinition *processingDefinition, QObject *parent) : QObject(parent)
{
   currentImageProcessor = nullptr;
   currentHxtGenerator = nullptr;
   this->processingDefinition = processingDefinition;
   nInRows = this->processingDefinition->getFrameInRows();
   nInCols = this->processingDefinition->getFrameInCols();
   nOutRows = this->processingDefinition->getFrameOutRows();
   nOutCols = this->processingDefinition->getFrameOutCols();
   pbgThread = new QThread();
   pbgThread->start();
   moveToThread(pbgThread);

   qDebug() << "(HxtGigE)PBG Start ThreadId: " <<QThread::currentThreadId();

   connect(this, SIGNAL(imageStarted(char*)),
           this, SLOT(handleImageStarted(char *)));
   connect(this, SIGNAL(fileBufferReady(unsigned char*, unsigned long)),
           this, SLOT(handleFileBufferReady(unsigned char *, unsigned long)));
   connect(this, SIGNAL(imageComplete(long long)),
           this, SLOT(handleImageComplete(long long)));

   bMainWindowBusy = false;
   saveRaw = true;
}

void ProcessingBufferGenerator::enqueueImage(const char *filename, int nInRows, int nInCols, ProcessingDefinition *processingDefinition)
{
   this->frameInSize = int(nInRows * nInCols * sizeof(uint16_t));

   currentImageProcessor = new ImageProcessor(filename, nInRows, nInCols, processingDefinition);
   currentImageProcessorHandler = new ImageProcessorHandler(currentImageProcessor);
   connect(currentImageProcessorHandler, SIGNAL(processingComplete()),
           this, SLOT(handleProcessingComplete()));
   connect(this, SIGNAL(mainWindowBusy(bool)),
           currentImageProcessor, SLOT(handleMainWindowBusy(bool)));

   /// Enable/disable saving raw file (GUI only - test.exe not affected)
   currentImageProcessor->setSaveRaw(saveRaw);

   HANDLE hxtHandle = currentImageProcessor->getHxtFileWrittenEvent();
   if (hxtHandle != nullptr)
   {
      hxtNotifier = new QWinEventNotifier(hxtHandle);
      connect(hxtNotifier, SIGNAL(activated(HANDLE)), this, SLOT(handleHxtFileWritten()));
   }

   /// Enable (current)ImageProcessor to signal occupancyCorrections, corrections
   connect(currentImageProcessor, SIGNAL(occupancyCorrections(int, int)),
           this, SLOT(handleOccupancyCorrections(int, int)));

//   currentHxtGenerator = currentImageProcessor->getHxtGenerator();
   qDebug() << "IMAGE QUEUED: currentImageProcessor " << currentImageProcessor;
}

void ProcessingBufferGenerator::handleOccupancyCorrections(int occupancyThreshold, int corrections)
{
   emit occupancyCorrections(occupancyThreshold, corrections);
}

void ProcessingBufferGenerator::handleSaveRawChanged(bool bSaveRaw)
{
   saveRaw = bSaveRaw;
}

void ProcessingBufferGenerator::handleImageStarted(char *filename)
{
   emit imageStarted();
   QMutexLocker locker(&mutex);
   enqueueImage(filename, processingDefinition->getFrameInRows(), processingDefinition->getFrameInCols(),
                processingDefinition);
}

void ProcessingBufferGenerator::handleFileBufferReady(unsigned char *fileBuffer, unsigned long validFrames)
{
   bufferReady(fileBuffer, validFrames);
   free(fileBuffer);
}

void ProcessingBufferGenerator::handleTransferBufferReady(unsigned char *transferBuffer, unsigned long validFrames)
{
   bufferReady(transferBuffer, validFrames);
   emit returnBufferReady(transferBuffer, validFrames);
}

void ProcessingBufferGenerator::bufferReady(unsigned char *buffer, unsigned long validFrames)
{
   bufferToProcess = (char *) malloc(frameInSize * validFrames);      // Free()'d by calling ImageProcessor::processThreshold...()
   memcpy(bufferToProcess, buffer, frameInSize * validFrames);
   currentImageProcessor->enqueueBuffer(bufferToProcess, validFrames);
}

void ProcessingBufferGenerator::handleImageComplete(long long totalFramesAcquired)
{
   currentImageProcessor->imageAcquisitionComplete(totalFramesAcquired);
}

void ProcessingBufferGenerator::handleConfigureProcessing(bool re_order, bool nextFrame,
                                                          int threshholdMode, int thresholdValue, QString thresholdFile)
{
   bool thresholdsStatus = true;
   bool gradientsStatus = true;
   bool interceptsStatus = true;

   processingDefinition->setRe_order(re_order);
   processingDefinition->setNextFrameCorrection(nextFrame);
   processingDefinition->setThresholdMode(ThresholdMode(threshholdMode));

   switch (ThresholdMode(threshholdMode))
   {
      case SINGLE_VALUE:
         processingDefinition->setThresholdValue(thresholdValue);
         break;
      case THRESHOLD_FILE:
         thresholdsStatus = processingDefinition->setThresholdPerPixel((char *)thresholdFile.toStdString().c_str());
         break;
      default:
         break;
   }
   if (!gradientsStatus || !interceptsStatus || !thresholdsStatus)
   {
      emit invalidParameterFiles(thresholdsStatus, gradientsStatus, interceptsStatus);
   }
}

void ProcessingBufferGenerator::handleConfigureProcessing(bool energyCalibration, bool hxtGeneration,
                                                          long long binStart, long long binEnd, double binWidth, bool totalSpectrum,
                                                          QString gradientFilename, QString interceptFilename)
{
   bool gradientsStatus = true;
   bool interceptsStatus = true;
   bool thresholdsStatus = true;

   processingDefinition->setEnergyCalibration(energyCalibration);
   processingDefinition->setHxtGeneration(hxtGeneration);
   processingDefinition->setBinStart(binStart);
   processingDefinition->setBinEnd(binEnd);
   processingDefinition->setBinWidth(binWidth);
   processingDefinition->setTotalSpectrum(totalSpectrum);
   gradientsStatus = processingDefinition->setGradientFilename((char *)gradientFilename.toStdString().c_str());
   interceptsStatus = processingDefinition->setInterceptFilename((char *)interceptFilename.toStdString().c_str());

   if (!gradientsStatus || !interceptsStatus || !thresholdsStatus)
   {
      emit invalidParameterFiles(thresholdsStatus, gradientsStatus, interceptsStatus);
   }
}

void ProcessingBufferGenerator::handleConfigureProcessing(int chargedSharingMode, int pixelGridOption)
{
   qDebug() << /*"ThreadID: " << QThread::currentThreadId() << */"ProcessingBufferGenerator::handleConfigureProcessing(int chargedSharingMode, int pixelGridOption: "
           << chargedSharingMode << pixelGridOption;
   processingDefinition->setChargedSharingMode(ChargedSharingMode(chargedSharingMode));
   processingDefinition->setPixelGridSize(pixelGridOption);
}

void ProcessingBufferGenerator::handleConfigureProcessing(QStringList inputFilesList,
                                                          QString outputDirectory, QString outputPrefix)
{
   this->inputFilesList = inputFilesList;
   processingDefinition->setOutputDirectory((char *)outputDirectory.toStdString().c_str());
   processingDefinition->setOutputPrefix((char *)outputPrefix.toStdString().c_str());

}

void ProcessingBufferGenerator::handleHxtFileWritten()
{
   bool bBusy = false;
   if (guiMutex.tryLock(100))
   {
      bBusy = bMainWindowBusy;
      guiMutex.unlock();
      emit mainWindowBusy(bBusy);
   }
   else
      qDebug() << " *** " << Q_FUNC_INFO << " Couldn't acquire  mutex!";

   if (!bBusy)
   {
      ///
      unsigned long runningAverageEvents = currentImageProcessor->getRunningAverageEvents();
//      qDebug() << "ThreadID: " << QThread::currentThreadId() << "PBG::hanHxtFileWritten, runningAverageEvents: " << runningAverageEvents;
      emit updateRunningAverageEvents(runningAverageEvents);

      /*qDebug() << "ThreadID: " << QThread::currentThreadId() << "PBG::hanHxtFileWritten; bMainWindowBusy: " << bMainWindowBusy;
      qDebug() << "\t ImageProcessor: " << currentImageProcessor << "\t hxtGenerator: " << currentHxtGenerator;*/
      currentHxtGenerator = currentImageProcessor->getHxtGenerator();   /// Moved from line: 64
      hxtFilename = QString(currentImageProcessor->getHxtFilename());
      char *buffer = (char *)currentHxtGenerator->getHxtV3Buffer();


      emit hxtFileWritten((unsigned short *)buffer, hxtFilename);  /// SLOT: MW::readBuffer(..)
   }
}

void ProcessingBufferGenerator::handleMainWindowBusy(bool bBusy)
{
   if (guiMutex.tryLock(100))
   {
      bMainWindowBusy = bBusy;
      emit mainWindowBusy(bMainWindowBusy);
      guiMutex.unlock();
   }
   else
   {
     qDebug() << " *** PBG Couldn't acquire mutex for bool = " << bBusy
              << " ! (It remains " << bMainWindowBusy << ")";
   }
}

void ProcessingBufferGenerator::handleConfigureSensor(int nInRows, int nInCols, int nOutRows, int nOutCols, int occupancyThreshold)
{
   long long frameSize;

   this->nInRows = nInRows;
   this->nInCols = nInCols;
   this->nOutRows = nOutRows;
   this->nOutCols = nOutCols;
   frameSize = (long long)nInRows * (long long)nInCols;
   processingDefinition->setFrameInRows(nInRows);
   processingDefinition->setFrameInCols(nInCols);
   processingDefinition->setFrameInSize(frameSize);
   processingDefinition->setFrameOutRows(nOutRows);
   processingDefinition->setFrameOutCols(nOutCols);
   frameSize = (long long)nOutRows * (long long)nOutCols;
   processingDefinition->setFrameOutSize(frameSize);
   processingDefinition->setOccupancyThreshold(occupancyThreshold);
}

void ProcessingBufferGenerator::handleProcessingComplete()
{
   qDebug() << "ProcessingBufferGenerator::handleProcessingComplete() called!!!";
   emit processingComplete();
}

void ProcessingBufferGenerator::handlePostProcessImages()
{
   int fileExtensionPos;
   QString outputFilename;
   unsigned char *transferBuffer;
   unsigned long validFrames = 0;
   long long totalFramesAcquired = 0;
   std::ifstream inFile;
   char *processingFilename;
   char *inputFilename;

   int nInRows = processingDefinition->getFrameInRows();
   int nInCols = processingDefinition->getFrameInCols();

   processingFilenameList.clear();

   inputFilename = new char[1024];

   foreach (const QString &str, inputFilesList)
   {
      QFileInfo fi(str);
      QString filename = fi.fileName();

      processingFilename = new char[1024];      // free()'d by ImageItem CTOR

      fileExtensionPos = filename.lastIndexOf(".");
      filename.truncate(fileExtensionPos);

      outputFilename = processingDefinition->getOutputDirectory();
      if (!outputFilename.endsWith("/"))
      {
         outputFilename.append("/");
      }
      outputFilename.append(processingDefinition->getOutputPrefix());
      outputFilename.append(filename);

      strcpy(processingFilename, (char *)outputFilename.toStdString().c_str());
      strcpy(inputFilename, (char *)str.toStdString().c_str());

      inFile.open(inputFilename, ifstream::binary);
      processingFilenameList.append(processingFilename);
      qDebug() << QThread::currentThreadId() << "  PBG file: " << processingFilename;
      emit imageStarted(processingFilenameList.back());
      if (!inFile)
      {
        qDebug() << "ProcessingBufferGenerator::handlePostProcessImages() - error opening " << inputFilename;
      }

      int bufferCount = 0;

      while (inFile)
      {
         transferBuffer = (unsigned char *) calloc(nInRows * nInCols * 500 * sizeof(uint16_t), sizeof(char));   // freed()'d in handleFileBufferReady
         /// transferBuffer passed onto ImageProcessor->enqueueBuffer(buffer, frames) via bufferReady(..)

         inFile.read((char *)transferBuffer, nInRows * nInCols  * 500 * 2);
         if (!inFile)
         {
            validFrames = inFile.gcount() / (nInRows * nInCols  * 2);
            emit fileBufferReady(transferBuffer, validFrames);
         }
         else
         {
            validFrames = inFile.gcount() / (nInRows * nInCols * 2);
            emit fileBufferReady(transferBuffer, 500);
         }
         totalFramesAcquired += validFrames;
         bufferCount++;
         Sleep(50);
      }
      inFile.close();
      qDebug() << QThread::currentThreadId() << " with: " << totalFramesAcquired << " frames sent for processing.";
      emit imageComplete(totalFramesAcquired);
      totalFramesAcquired = 0;
   }
   delete inputFilename;
}
