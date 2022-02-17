#include "processingbuffergenerator.h"

#include <QSettings>
#include <QMutexLocker>
#include <QFileInfo>
#include <QDebug>
#include <QThread>
#include <iostream>
#include <fstream>
#include <Windows.h>
//
#include <QTime>

ProcessingBufferGenerator::ProcessingBufferGenerator(ProcessingDefinition *processingDefinition, QObject *parent) : QObject(parent)
{
   currentImageProcessor = NULL;
   this->processingDefinition = processingDefinition;
   nInRows = this->processingDefinition->getFrameInRows();
   nInCols = this->processingDefinition->getFrameInCols();

   pbgThread = new QThread();
   pbgThread->start();
   moveToThread(pbgThread);

   qDebug() << "(HxtGigE)PBG Start ThreadId: " <<QThread::currentThreadId() << " rows, columns: " << nInRows << nInCols;

   connect(this, SIGNAL(imageStarted(char*)),
           this, SLOT(handleImageStarted(char *)));
   connect(this, SIGNAL(fileBufferReady(unsigned char*, unsigned long)),
           this, SLOT(handleFileBufferReady(unsigned char *, unsigned long)));
   connect(this, SIGNAL(imageComplete(long long)),
           this, SLOT(handleImageComplete(long long)));
}

void ProcessingBufferGenerator::enqueueImage(const char *filename, int nInRows, int nInCols, int nOutRows, int nOutCols, ProcessingDefinition *processingDefinition)
{
   this->frameInSize = nInRows * nInCols * sizeof(uint16_t);
   this->frameOutSize = nOutRows * nOutCols * sizeof(uint16_t);
   qDebug() << "PBG::enqueueImage(..) rows, columns, frameSize: " << nInRows << nInCols << nOutRows << nOutCols << frameInSize << frameOutSize;
   currentImageProcessor = new ImageProcessor(filename, nInRows, nInCols, nOutRows, nOutCols, processingDefinition);
   currentImageProcessorHandler = new ImageProcessorHandler(currentImageProcessor);
   connect(currentImageProcessorHandler, SIGNAL(processingComplete()),
           this, SLOT(handleProcessingComplete()));

   HANDLE hxtHandle = currentImageProcessor->getHxtFileWrittenEvent();
   if (hxtHandle != NULL)
   {
      hxtNotifier = new QWinEventNotifier(hxtHandle);
      connect(hxtNotifier, SIGNAL(activated(HANDLE)), this, SLOT(handleHxtFileWritten()));
   }

   currentHxtGenerator = currentImageProcessor->getHxtGenerator();
   qDebug() << "IMAGE QUEUED: currentImageProcessor " << currentImageProcessor;
}


void ProcessingBufferGenerator::handleImageStarted(char *filename)
{
   QMutexLocker locker(&mutex);
   enqueueImage(filename, processingDefinition->getFrameInRows(), processingDefinition->getFrameInCols(),
               processingDefinition->getFrameOutRows(), processingDefinition->getFrameOutCols(), processingDefinition);
}

void ProcessingBufferGenerator::handleFileBufferReady(unsigned char *fileBuffer, unsigned long validFrames)
{
   bufferReady(fileBuffer, validFrames);
   free(fileBuffer);
   fileBuffer = NULL;
}

void ProcessingBufferGenerator::bufferReady(unsigned char *buffer, unsigned long validFrames)
{
   bufferToProcess = (char *) malloc(frameInSize * validFrames);          // free()'d in imageprocessor::processThreshol…() functions
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
   processingDefinition->setThresholdMode((ThresholdMode)threshholdMode);

   switch ((ThresholdMode)threshholdMode)
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
   processingDefinition->setChargedSharingMode((ChargedSharingMode)chargedSharingMode);
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
   hxtFilename = QString(currentImageProcessor->getHxtFilename());
   char *buffer = (char *)currentHxtGenerator->getHxtV3Buffer();

   emit hxtFileWritten((unsigned short *)buffer, hxtFilename);
}

void ProcessingBufferGenerator::handleConfigureProcessing(int nInRows, int nInCols, int nOutRows, int nOutCols, long long frameInSize, long long frameOutSize)
{
   /// NEVER ENGAGED
   qDebug() << "PBG::handle..:157 rows, columns, frameInSize = " << nInRows << nInCols << frameInSize << nOutRows << nOutCols << frameOutSize;
   processingDefinition->setFrameInRows(nInRows);
   processingDefinition->setFrameInCols(nInCols);
   processingDefinition->setFrameInSize(frameInSize);
   processingDefinition->setFrameOutRows(nOutRows);
   processingDefinition->setFrameOutCols(nOutCols);
   processingDefinition->setFrameOutSize(frameOutSize);
}

void ProcessingBufferGenerator::handleConfigureSensor(int nInRows, int nInCols, int nOutRows, int nOutCols)
{
   qDebug() << "PBG::handle..:165 rows, columns = " << nInRows << nInCols << frameInSize << nOutRows << nOutCols << frameOutSize;
   this->nInRows = nInRows;
   this->nInCols = nInCols;
   this->nOutRows = nOutRows;
   this->nOutCols = nOutCols;
   processingDefinition->setFrameInRows(nInRows);
   processingDefinition->setFrameInCols(nInCols);
   processingDefinition->setFrameInSize(nInRows * nInCols);
   processingDefinition->setFrameOutRows(nOutRows);
   processingDefinition->setFrameOutCols(nOutCols);
   processingDefinition->setFrameOutSize(nOutRows * nOutCols);
}

void ProcessingBufferGenerator::handleProcessingComplete()
{
   emit processingComplete();   // Calls processingForm::guiIdle()
}

// Called when Test.exe processes data
void ProcessingBufferGenerator::handlePostProcessImages(int nInRows, int nInCols, int nOutRows, int nOutCols)
{
   int fileExtensionPos;
   QString outputFilename;
   unsigned char *transferBuffer;
   unsigned long validFrames = 0;
   long long totalFramesAcquired = 0;
   std::ifstream inFile;
   char *processingFilename;
   char *inputFilename;

   processingDefinition->setFrameInRows(nInRows);
   processingDefinition->setFrameInCols(nInCols);
   processingDefinition->setFrameInSize(nInRows * nInCols);
   processingDefinition->setFrameOutRows(nOutRows);
   processingDefinition->setFrameOutCols(nOutCols);
   processingDefinition->setFrameOutSize(frameOutSize);
   processingFilenameList.clear();

//   QTime qtTime;
//   int readTime = 0, readyTime = 0;

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

      strcpy(processingFilename, (char *)outputFilename.toStdString().c_str());
      strcpy(inputFilename, (char *)str.toStdString().c_str());

      inFile.open(inputFilename, ifstream::binary);
      processingFilenameList.append(processingFilename);

      emit imageStarted(processingFilenameList.back()); // Passed via ::enqueueImage() into ImageItem() CTOR
      if (!inFile)
      {
        qDebug() << "ProcessingBufferGenerator::handlePostProcessImages() - error opening " << inputFilename;
      }
      qDebug() << "PBG::handle..:228 rows, columns = " << nInRows << nInCols << frameInSize << nOutRows << nOutCols << frameOutSize;
      while (inFile)
      {
         transferBuffer = (unsigned char *) calloc(nInRows * nInCols * 500 * sizeof(uint16_t), sizeof(char)); // free()'d in ::handleFileBufferReady

         inFile.read((char *)transferBuffer, nInRows * nInCols  * 500 * 2);
         if (!inFile)
         {
            validFrames = inFile.gcount() / (nInRows * nInCols  * 2);
            emit fileBufferReady(transferBuffer, validFrames);
         }
         else
         {
            validFrames = inFile.gcount() / (nInRows * nInCols  * 2);
            emit fileBufferReady(transferBuffer, 500);
         }
         totalFramesAcquired += validFrames;
         Sleep(50);
      }
      inFile.close();
      qDebug() << "PBG read " << totalFramesAcquired << " frames from: " << filename;
      emit imageComplete(totalFramesAcquired);
      totalFramesAcquired = 0;
   }
   //  Freeing these locally assigned pointers causes crashes occasionally:
//   delete processingFilename;
//   delete inputFilename;

}
