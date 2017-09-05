#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include "bufferitem.h"
#include <QMutex>
#include <QQueue>

class ImageItem
{
public:
   ImageItem(const char *filename, int nRows, int nCols);
   ~ImageItem();
   void enqueueBuffer(char *address, unsigned long validFrameCount);
   char *getNextBuffer(unsigned long *validFrameCount);
   int getBufferQueueSize();
   char *getFilename();

private:
   QMutex mutex;
   char *filename;
   int frameSize;
   BufferItem *bufferItem;
   QQueue <BufferItem *>bufferQueue;
   unsigned long processedFrameCount;
};

#endif // IMAGEITEM_H
