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
   void imageAcquisitionComplete(long long totalFramesAcquired);
   void setImageInProgress(bool inProgress);

   //MAKE imageItem private
   ImageItem *imageItem;

   
private:
   void writeFile(char *buffer, unsigned long length, const char* filename);
   QThread *imageProcessorThread;
   ProcessingDefinition *processingDefinition;
   GeneralHxtGenerator *hxtGenerator;
   bool energyCalibration;
   double *pixelEnergy;

//   ImageItem *imageItem;
   char *bufferToProcess;
   int frameSize;
   long long totalFramesToProcess;
   long long processedFrameCount;
   bool inProgress;

   void processThresholdNone(GeneralFrameProcessor *fp, uint16_t *result, const char* filename);
   void processThresholdValue(GeneralFrameProcessor *fp, int thresholdValue, uint16_t *result, const char* filename);
   void processThresholdFile(GeneralFrameProcessor *fp, uint16_t *thresholdPerPixel, uint16_t *result, const char* filename);

signals:
   void process();
   void imageComplete(long long processedFrameCount);
   void processingComplete(ImageProcessor *completedImageProcessor, long long processedFrameCount);
   void returnBufferReady(char *transferBuffer);

public slots:
   void handleProcess();
};

#endif // IMAGEPROCESSOR_H
