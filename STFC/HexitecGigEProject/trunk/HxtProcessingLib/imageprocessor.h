#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include "imageprocessor.h"
#include "processingdefinition.h"
#include "imageitem.h"
#include "frameprocessor.h"
#include "framere_orderprocessor.h"
#include "hxtgenerator.h"
#include "hxtchargedsharinggenerator.h"
#include "hxttotalspectrumgenerator.h"
#include <QObject>

using namespace std;

class ImageProcessor: public QObject
{
   Q_OBJECT

public:
   ImageProcessor(const char *name, int nRows, int nCols, ProcessingDefinition *processingDefinition);
   ~ImageProcessor();
   void enqueueBuffer(char *transferBuffer, unsigned long validFrames);
   void imageAcquisitionComplete(long long totalFramesAcquired);
   void setImageInProgress(bool inProgress);

   //MAKE imageItem private
   ImageItem *imageItem;

   
private:
   void writeBinFile(char *buffer, unsigned long length, const char* filename);
   void writeHxtFile(char *header, unsigned long headerLength, char *data, unsigned long dataLength, const char* filename);
   void writeCsvFile(double *energyBin, long long *summedHistogram, const char *filename);
   QThread *imageProcessorThread;
   ProcessingDefinition *processingDefinition;
   GeneralHxtGenerator *hxtGenerator;
   bool energyCalibration;
   bool nextFrameCorrection;
   bool chargedSharing;
   double *pixelEnergy;
   unordered_map<int, double> *pixelEnergyMap;

//   ImageItem *imageItem;
   char *bufferToProcess;
   int frameSize;
   long long totalFramesToProcess;
   long long processedFrameCount;
   bool inProgress;

   void processThresholdNone(GeneralFrameProcessor *fp, uint16_t *result,
                             const char* filenameBin, const char *filenameHxt, const char *filenameCsv);
   void processThresholdValue(GeneralFrameProcessor *fp, int thresholdValue, uint16_t *result,
                              const char* filenameBin, const char *filenameHxt, const char *filenameCsv);
   void processThresholdFile(GeneralFrameProcessor *fp, uint16_t *thresholdPerPixel,
                             uint16_t *result, const char* filenameBin, const char *filenameHxt, const char *filenameCsv);

signals:
   void process();
   void imageComplete(long long processedFrameCount);
   void hxtFileWritten(char *buffer, const char*filename);
   void processingComplete(ImageProcessor *completedImageProcessor, long long processedFrameCount);
   void returnBufferReady(char *transferBuffer);

public slots:
   void handleProcess();
};

#endif // IMAGEPROCESSOR_H
