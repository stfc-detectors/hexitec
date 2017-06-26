#ifndef HXTGENERATORTESTER_H
#define HXTGENERATORTESTER_H

#include "hxtgenerator.h"
#include "processingdefinition.h"
#include <QObject>

class HxtGeneratorTester : public QObject
{
   Q_OBJECT
public:
   HxtGeneratorTester(HxtGenerator *hxtGenerator, QObject *parent = 0);
   void processImage(const char *imageFilename, const char *filename);

private:
   ProcessingDefinition *processingDefinition;

signals:
   void imageStarted(const char *path, int frameSize, ProcessingDefinition * processingDefinition);
   void transferBufferReady(char *transferBuffer, unsigned long validFrames);
   void imageComplete(unsigned long long totalFramesAcquired);
};

#endif // HXTGENERATORTESTER_H
