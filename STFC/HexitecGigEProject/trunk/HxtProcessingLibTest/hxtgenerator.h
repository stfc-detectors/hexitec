#ifndef HXTGENERATOR_H
#define HXTGENERATOR_H

#include "imageprocessor.h"
#include <QObject>
#include <QQueue>
#include <cstdint>

class HxtGenerator : public QObject
{
   Q_OBJECT
public:
   explicit HxtGenerator(ProcessingDefinition *processingDefinition, QObject *parent = 0);
   void enqueueImage(const char *name, int frameSize, ProcessingDefinition *processingDefinition);

private:
   QQueue<ImageProcessor *>imageProcessorQueue;
   ImageProcessor *currentImageProcessor;
   ProcessingDefinition *processingDefinition;
   char *bufferToProcess;
   int frameSize;

signals:
   void enqueueBuffer (char *bufferToProcess, unsigned long validFrames);
   void imageComplete(unsigned long long totalFramesAcquired);

public slots:
   void handleImageStarted(const char *path, int frameSize);
   void handleTransferBufferReady(char *transferBuffer, unsigned long validFrames);
   void handleImageComplete(unsigned long long totalFramesAcquired);
   void handleConfigureProcessing(int threshholdMode,
                            int thresholdValue,
                            uint16_t *thresholdPerPixel,
                            const char *gradientFilename,
                            const char *interceptFilename,
                            const char *processedFilename);
};

#endif // HXTGENERATOR_H
