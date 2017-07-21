#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include "imageprocessor.h"
#include "processingdefinition.h"
#include "imageitem.h"
#include "frameprocessor.h"
#include "framere_orderprocessor.h"
#include "hxtgenerator.h"
#include "hxttotalspectrumgenerator.h"
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
   void setAcquisitionInProgress(bool acquisitionInProgress);
   void setProcessingInProgress(bool processingInProgress);
//   void imageComplete(unsigned long long totalFramesToProcess);
   //MAKE imageItem private
   ImageItem *imageItem;

   
private:
   void writeFile(char *buffer, unsigned long length, const char* filename);
   QThread *imageProcessorThread;
   ProcessingDefinition *processingDefinition;
   GeneralHxtGenerator *hxtGenerator;

//   ImageItem *imageItem;
   char *bufferToProcess;
   int frameSize;
   unsigned long long totalFramesToProcress;
   unsigned long long processedFrameCount;
   bool acquisitionInProgress;
   bool processingInProgress;

   void processThresholdNone(GeneralFrameProcessor *fp, uint16_t *result, const char* filename);
   void processThresholdValue(GeneralFrameProcessor *fp, int thresholdValue, uint16_t *result, const char* filename);
   void processThresholdFile(GeneralFrameProcessor *fp, uint16_t *thresholdPerPixel, uint16_t *result, const char* filename);

signals:
   void process();
   void processingComplete(ImageProcessor *completedImageProcessor, unsigned long long processedFrameCount);
   void returnBufferReady(char *transferBuffer);

public slots:
   void handleProcess();
   void handleEnergyProcessingComplete(unsigned long long processedEnergyCount);
};

#endif // IMAGEPROCESSOR_H
