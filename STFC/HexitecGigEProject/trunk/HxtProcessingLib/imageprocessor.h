#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include "imageprocessor.h"
#include "processingdefinition.h"
#include "imageitem.h"
#include "frameprocessor.h"
#include "framere_orderprocessor.h"
#include "hxtgenerator.h"
#include "hxtsumgenerator.h"
#include "hxtchargedsharinggenerator.h"
#include "hxtchargedsharingsumgenerator.h"
#include <windows.h>

using namespace std;

#define IMAGE_COMPLETE TEXT("ImageComplete")
#define PROCESSING_COMPLETE TEXT("ProcessingComplete")
#define HXT_FILE_WRITTEN TEXT("HxtFileWritten")
static HANDLE imageCompleteEvent;
static HANDLE processingCompleteEvent;
static HANDLE hxtFileWrittenEvent;

class ImageProcessor
{

public:
   HANDLE getProcessingCompleteEvent();
   HANDLE getImageCompleteEvent();
   HANDLE getHxtFileWrittenEvent();

   ImageProcessor(const char *name, int nRows, int nCols, ProcessingDefinition *processingDefinition);
   ~ImageProcessor();
   void freeAllocedMemory();
   void enqueueBuffer(char *transferBuffer, unsigned long validFrames);
   void imageAcquisitionComplete(long long totalFramesAcquired);
   void setImageInProgress(bool inProgress);
   void handleProcess();
   char *getHxtFilename();
   GeneralHxtGenerator *getHxtGenerator();

   //MAKE imageItem private
   ImageItem *imageItem;

   
private:
   void writeBinFile(char *buffer, unsigned long length, const char* filename);
   void writeHxtFile(char *header, unsigned long headerLength, char *data, unsigned long dataLength, const char* filename);
   void writeCsvFile(double *energyBin, long long *summedHistogram, const char *filename);
   ProcessingDefinition *processingDefinition;
   GeneralHxtGenerator *hxtGenerator;
   bool energyCalibration;
   bool hxtGeneration;
   bool nextFrameCorrection;
   bool chargedSharing;
   bool totalSpectrum;
   double *pixelEnergy;
   unordered_map<int, double> *hxtMap;
   char* filenameBin;
   char* filenameHxt;
   char* filenameCsv;

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

};

#endif // IMAGEPROCESSOR_H
