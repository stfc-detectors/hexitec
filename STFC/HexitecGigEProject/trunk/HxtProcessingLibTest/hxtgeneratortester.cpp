#include "hxtgeneratortester.h"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <QDebug>

using namespace std;

HxtGeneratorTester::HxtGeneratorTester(HxtGenerator *hxtGenerator, QObject *parent) : QObject(parent)
{
   connect(this, SIGNAL(imageStarted(const char*, int, ProcessingDefinition *)),
           hxtGenerator, SLOT(handleImageStarted(const char *, int, ProcessingDefinition *)));
   connect(this, SIGNAL(transferBufferReady(char*, unsigned long)),
           hxtGenerator, SLOT(handleTransferBufferReady(char *, unsigned long)));
   connect(this, SIGNAL(imageComplete(unsigned long long)),
           hxtGenerator, SLOT(handleImageComplete(unsigned long long)));
   processingDefinition = new ProcessingDefinition();
}

void HxtGeneratorTester::processImage(const char *imageFilename, const char *filename)
{
   char *transferBuffer;
   unsigned long validFrames = 0;
   unsigned long long totalFramesAcquired = 0;
   std::ifstream inFile;

   inFile.open(filename, ifstream::binary);
   emit imageStarted(imageFilename, 6400 * 2, processingDefinition);

   while (inFile)
   {
      transferBuffer = (char *) calloc(6400 * 1000, sizeof(uint16_t));

      inFile.read((char *)&transferBuffer[0], 6400 * 500 * 2);
      if (!inFile)
      {
         validFrames = inFile.gcount() / (6400 * 2);
         qDebug() << "error: only " << validFrames << " valid frames could be read";
         emit transferBufferReady(transferBuffer, validFrames);
      }
      else
      {
         validFrames = inFile.gcount() / (6400 * 2);
         qDebug() << "frame read OK " << validFrames << " valid frames read";
         emit transferBufferReady(transferBuffer, 500);
      }
      totalFramesAcquired += validFrames;
   }
   inFile.close();
   emit imageComplete(totalFramesAcquired);

//   fp = new FrameProcessor();

//   fp->process(frame);

}

/*
void HxtGeneratorTester::configureProcessing(ThresholdMode threshholdMode, int thresholdValue, uint16_t *thresholdPerPixel,
                                             const char *gradientFilename,
                                             const char *interceptFilename,
                                             const char *processedFilename)
{
   processingDefinition->setThresholdMode(threshholdMode);
   processingDefinition->setThresholdValue(thresholdValue);
   processingDefinition->setThresholdPerPixel(thresholdPerPixel);
   processingDefinition->setGradientFilename((char *)gradientFilename);
   processingDefinition->setInterceptFilename((char *)interceptFilename);
   processingDefinition->setProcessedFilename((char *)processedFilename);
}
*/
