#ifndef PROCESSINGBUFFERGENERATOR_H
#define PROCESSINGBUFFERGENERATOR_H

#include "imageprocessor.h"
#include "inifile.h"
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
   void bufferReady(unsigned char *buffer, unsigned long validFrames);
   IniFile *twoEasyIniFile;
   QMutex mutex;
   QList<ImageProcessor *>imageProcessorList;
   QList<char *>processingFilenameList;
   ImageProcessor *currentImageProcessor;
   QStringList inputFilesList;
   ProcessingDefinition *processingDefinition;
   QString hxtFilename;
   char *bufferToProcess;
   int frameSize;
   int nRows;
   int nCols;

signals:
//   void enqueueBuffer (char *bufferToProcess, unsigned long validFrames);
//   void imageComplete(unsigned long long totalFramesAcquired);
//   void imageStarted(const char *path, int nRows, int nCols);
   void imageStarted(char *path);
   void fileBufferReady(unsigned char *fileBuffer, unsigned long validFrames);
   void returnBufferReady(unsigned char *transferBuffer, unsigned long validFrames);
   void imageComplete(long long totalFramesAcquired);
   void hxtFileWritten(unsigned short *buffer, QString filename);

public slots:
   void handleProcessImages();
   void handlePostProcessImages(int nRows, int nCols);
   void handleImageStarted(char *filename);
//   void handleTransferBufferReady(unsigned char *transferBuffer, unsigned long validFrames, int mode);
   void handleTransferBufferReady(unsigned char *transferBuffer, unsigned long validFrames);
   void handleFileBufferReady(unsigned char *fileBuffer, unsigned long validFrames);
   void handleImageComplete(long long totalFramesAcquired);
   void handleHxtFileWritten(char *buffer, const char * filename);

   void handleConfigureProcessing(int nRows, int nCols,
                                  long long frameSize);
   void handleConfigureProcessing(bool re_order,
                                  bool nextFrame,
                                  int threshholdMode,
                                  int thresholdValue,
                                  QString thresholdFilname);
   void handleConfigureProcessing(bool energyCalibration,
                                  bool hxtGeneration,
                                  long long binStart,
                                  long long binEnd,
                                  double binWidth,
                                  bool totalSpectrum,
                                  QString gradientFilename,
                                  QString interceptFilename);
   void handleConfigureProcessing(int chargedSharingMode,
                                  int pixelGridOption);
   void handleConfigureProcessing(QStringList inputFilesList,
                                  QString outputDirectory,
                                  QString outputPrefix);
   void handleProcessingComplete(ImageProcessor *completedImageProcessor, long long processedFrameCount);
};

#endif // PROCESSINGBUFFERGENERATOR_H
