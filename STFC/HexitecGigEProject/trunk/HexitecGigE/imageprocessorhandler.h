#ifndef IMAGEPROCESSORHANDLER_H
#define IMAGEPROCESSORHANDLER_H

#include "imageprocessor.h"
#include <QObject>
#include <QThread>

class ImageProcessorHandler  : public QThread
{
public:
   ImageProcessorHandler(ImageProcessor *imageProcessor);

private:
   ImageProcessor *imageProcessor;
   void run();
};

#endif // IMAGEPROCESSORHANDLER_H
