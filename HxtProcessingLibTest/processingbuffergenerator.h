#ifndef PROCESSINGBUFFERGENERATOR_H
#define PROCESSINGBUFFERGENERATOR_H

#include "imageprocessor.h"
#include "imageprocessorhandler.h"
#include "hxtgenerator.h"
#include "inifile.h"
#include <QObject>
#include <QList>
#include <QMutex>
#include <QThread>
#include <QWinEventNotifier>
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
   QThread *pbgThread;
   QMutex mutex;
   QList<char *>processingFilenameList;
   ImageProcessor *currentImageProcessor;
   ImageProcessorHandler *currentImageProcessorHandler;
   GeneralHxtGenerator *currentHxtGenerator;
   QStringList inputFilesList;
   ProcessingDefinition *processingDefinition;
   QString hxtFilename;
   char *bufferToProcess;
   int frameSize;
   int nRows;
   int nCols;
   QWinEventNotifier *hxtNotifier;


signals:
   void imageStarted(char *path);
   void fileBufferReady(unsigned char *fileBuffer, unsigned long validFrames);
   void returnBufferReady(unsigned char *transferBuffer, unsigned long validFrames);
   void imageComplete(long long totalFramesAcquired);
   void processingComplete();
   void hxtFileWritten(unsigned short *buffer, QString filename);
   void invalidParameterFiles(bool thresholdsStatus, bool gradientsStatus, bool interceptsStatus);

public slots:
   void handlePostProcessImages(int nRows, int nCols);
   void handleImageStarted(char *filename);
   void handleFileBufferReady(unsigned char *fileBuffer, unsigned long validFrames);
   void handleImageComplete(long long totalFramesAcquired);
   void handleProcessingComplete();
   void handleHxtFileWritten();

   void handleConfigureSensor(int nRows, int nCols);
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
};

#endif // PROCESSINGBUFFERGENERATOR_H
