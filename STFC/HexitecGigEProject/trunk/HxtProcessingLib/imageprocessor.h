#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include "imageprocessor.h"
#include "processingdefinition.h"
#include "imageitem.h"
#include <QObject>

using namespace std;

class ImageProcessor: public QObject
{
   Q_OBJECT

public:
   ImageProcessor(const char *name, int frameSize, ProcessingDefinition *processingDefinition);
   ~ImageProcessor();
   void enqueueBuffer(char *transferBuffer, unsigned long validFrames);
   void imageComplete(unsigned long long totalFramesAcquired);
//   void enqueue(char *bufferToProcess, unsigned long validFrames);
   void setImageInProgress(bool inProgress);
//   void imageComplete(unsigned long long totalFramesToProcess);
   //MAKE imageItem private
   ImageItem *imageItem;

private:
//   void process();
   QThread *imageProcessorThread;
   ProcessingDefinition *processingDefinition;
//   ImageItem *imageItem;
   char *bufferToProcess;
   int frameSize;
   unsigned long long totalFramesToProcress;
   unsigned long long processedFrameCount;
   bool inProgress;

signals:
   void process();
   void processingComplete(ImageProcessor *completedImageProcessor, unsigned long long processedFrameCount);
   void returnBufferReady(char *transferBuffer);

public slots:
//   void handleEnqueueBuffer(char *transferBuffer, unsigned long validFrames);
//   void handleImageComplete(unsigned long long totalFramesAcquired);
   void handleProcess();
};

#endif // IMAGEPROCESSOR_H
