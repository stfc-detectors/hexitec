#ifndef PROCESSINGBUFFERGENERATOR_H
#define PROCESSINGBUFFERGENERATOR_H

#include "imageprocessor.h"
#include <QObject>
#include <QList>
#include <QMutex>
#include <cstdint>

class ProcessingBufferGenerator : public QObject
{
   Q_OBJECT
public:
   explicit ProcessingBufferGenerator(ProcessingDefinition *processingDefinition, QObject *parent = 0);
   void enqueueImage(const char *filename, int nRows, int nCols, ProcessingDefinition *processingDefinition);

private:
   QMutex mutex;
   QList<ImageProcessor *>imageProcessorList;
   QList<char *>processingFilenameList;
   ImageProcessor *currentImageProcessor;
   QStringList inputFilesList;
   ProcessingDefinition *processingDefinition;
   char *bufferToProcess;
   int frameSize;

signals:
//   void enqueueBuffer (char *bufferToProcess, unsigned long validFrames);
//   void imageComplete(unsigned long long totalFramesAcquired);
   void imageStarted(const char *path, int nRows, int nCols);
   void transferBufferReady(char *transferBuffer, unsigned long validFrames);
   void imageComplete(long long totalFramesAcquired);

public slots:
   void handleProcessImages();
   void handleImageStarted(const char *filename, int nRows, int nCols);
   void handleTransferBufferReady(char *transferBuffer, unsigned long validFrames);
   void handleImageComplete(long long totalFramesAcquired);

   void handleConfigureProcessing(bool re_order,
                                  bool nextFrame,
                                  int threshholdMode,
                                  int thresholdValue,
                                  const char *thresholdFilname);
   void handleConfigureProcessing(bool energyCalibration,
                                  long long binStart,
                                  long long binEnd,
                                  double binWidth,
                                  bool totalSpectrum,
                                  char *gradientFilename,
                                  char *interceptFilename,
                                  const char *processedFilename);
   void handleConfigureProcessing(int chargedSharingMode, int pixelGridOption);
   void handleConfigureProcessing(QStringList inputFilesList,
                                  QString outputDirectory, QString outputPrefix);
   void handleProcessingComplete(ImageProcessor *completedImageProcessor, long long processedFrameCount);
};

#endif // PROCESSINGBUFFERGENERATOR_H
