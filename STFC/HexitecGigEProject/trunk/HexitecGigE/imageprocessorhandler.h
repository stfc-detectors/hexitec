#ifndef IMAGEPROCESSORHANDLER_H
#define IMAGEPROCESSORHANDLER_H

#include "imageprocessor.h"
#include <QObject>
#include <QThread>

class ImageProcessorHandler  : public QThread
{
   Q_OBJECT

public:
   ImageProcessorHandler(ImageProcessor *imageProcessor);

signals:
   void processingComplete();

private:
   ImageProcessor *imageProcessor;
   void run();
};

#endif // IMAGEPROCESSORHANDLER_H
