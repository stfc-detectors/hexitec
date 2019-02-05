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
#include <QObject>
// Needed for waitWhileGuiRefreshesDisplay(), bMainWindowBusy
#include <QMutex>

using namespace std;

#define IMAGE_COMPLETE TEXT("ImageComplete")
#define PROCESSING_COMPLETE TEXT("ProcessingComplete")
#define HXT_FILE_WRITTEN TEXT("HxtFileWritten")
static HANDLE imageCompleteEvent;
static HANDLE processingCompleteEvent;
static HANDLE hxtFileWrittenEvent;

class ImageProcessor : public QObject
{
   Q_OBJECT
public:
   HANDLE getProcessingCompleteEvent();
   HANDLE getImageCompleteEvent();
   HANDLE getHxtFileWrittenEvent();

   ImageProcessor(const char *name, int nRows, int nCols, ProcessingDefinition *processingDefinition);
   ~ImageProcessor();
//   void freeAllocedMemory();
   void enqueueBuffer(char *transferBuffer, unsigned long validFrames);
   void imageAcquisitionComplete(long long totalFramesAcquired);
   void setImageInProgress(bool inProgress);
   void handleProcess();
   char *getHxtFilename();
   GeneralHxtGenerator *getHxtGenerator();
   /// Allow User (Interface) to [not] save raw data file(s)
   void setSaveRaw(bool bSaveRaw);
   unsigned long getRunningAverageEvents();

private:
   void writeBinFile(char *buffer, unsigned long length, const char* filename);
   void writeHxtFile(char *header, unsigned long headerLength, char *data, unsigned long dataLength, const char* filename);
   void writeCsvFile(double *energyBin, long long *summedHistogram, const char *filename);
   ProcessingDefinition *processingDefinition;
   GeneralHxtGenerator *hxtGenerator;
   /// For HexitecGigE, this->hxtGeneration->hxtItem mustn't be deleted while MainWindow accessing
   ///  it whilst updating the display (i.e. don't delete hxtGenerator prematurely)
  /// NOTE: Not freeing hxtGenerator; The resulting memory leak prevents GUI crashing
   bool bMainWindowBusy;
   QMutex waitMutex;
   QMutex eventsMutex;

   bool energyCalibration;
   bool hxtGeneration;
   bool nextFrameCorrection;
   bool chargedSharing;
   bool totalSpectrum;
   unordered_map<int, double> *hxtMap;
   char* filenameBin;
   char* filenameHxt;
   char* filenameCsv;

   ImageItem *imageItem;
   char *bufferToProcess;
   int frameSize;
   long long totalFramesToProcess;
   long long processedFrameCount;
   bool inProgress;

   void processThresholdNone(GeneralFrameProcessor *fp, double *result,
                             const char* filenameBin, const char *filenameHxt, const char *filenameCsv);
   void processThresholdValue(GeneralFrameProcessor *fp, int thresholdValue, double *result,
                              const char* filenameBin, const char *filenameHxt, const char *filenameCsv);
   void processThresholdFile(GeneralFrameProcessor *fp, uint16_t *thresholdPerPixel,
                             double *result, const char* filenameBin, const char *filenameHxt, const char *filenameCsv);
   bool saveRaw;
   ///
   int occupancyThreshold;
   unsigned long runningAverageEvents;

public slots:
   void handleMainWindowBusy(bool bBusy);

signals:
   void occupancyCorrections(int threshold, int corrections);

};

#endif // IMAGEPROCESSOR_H
