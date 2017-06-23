#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include "bufferitem.h"
#include <QQueue>

class ImageItem
{
public:
   ImageItem(const char *name, int frameSize);
   void enqueueBuffer(char *address, unsigned long validFrameCount);
   char *getNextBuffer(unsigned long *validFrameCount);
   int getBufferQueueSize();

private:
   char *name;
   int frameSize;
   QQueue <BufferItem *>bufferQueue;
   unsigned long processedFrameCount;
};

#endif // IMAGEITEM_H
