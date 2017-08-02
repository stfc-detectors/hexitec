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
   void enqueueImage(const char *name, int frameSize, ProcessingDefinition *processingDefinition);

private:
   QMutex mutex;
   QList<ImageProcessor *>imageProcessorList;
   ImageProcessor *currentImageProcessor;
   ProcessingDefinition *processingDefinition;
   char *bufferToProcess;
   int frameSize;

signals:
//   void enqueueBuffer (char *bufferToProcess, unsigned long validFrames);
//   void imageComplete(unsigned long long totalFramesAcquired);

public slots:
   void handleImageStarted(const char *path, int frameSize);
   void handleTransferBufferReady(char *transferBuffer, unsigned long validFrames);
   void handleImageComplete(long long totalFramesAcquired);
   void handleConfigureProcessing(bool re_order,
                                  const char *gradientFilename,
                                  const char *interceptFilename,
                                  const char *processedFilename);
   void handleConfigureProcessing(int threshholdMode,
                                  int thresholdValue,
                                  uint16_t *thresholdPerPixel,
                                  const char *gradientFilename,
                                  const char *interceptFilename,
                                  const char *processedFilename);
   void handleConfigureProcessing(bool energyCalibration,
                                  long long binStart,
                                  long long binEnd,
                                  long long binWidth,
                                  bool totalSpectrum,
                                  const char *gradientFilename,
                                  const char *interceptFilename,
                                  const char *processedFilename);
   void handleProcessingComplete(ImageProcessor *completedImageProcessor, long long processedFrameCount);
};

#endif // PROCESSINGBUFFERGENERATOR_H
