#include "imageitem.h"
#include <QMutexLocker>
#include <QDebug>
#include <QThread>
#include <iostream>

using namespace std;

ImageItem::ImageItem(const char *name, int frameSize)
{
   QMutexLocker locker(&mutex);
   this->name = (char *)name;
   this->frameSize = frameSize;
   this->bufferQueue.clear();
   this->processedFrameCount = 0;
}

void ImageItem::enqueueBuffer(char *address, unsigned long validFrameCount)
{
   QMutexLocker locker(&mutex);
   bufferQueue.enqueue(new BufferItem(address, validFrameCount));
}

char *ImageItem::getNextBuffer(unsigned long *validFrameCount)
{
   QMutexLocker locker(&mutex);
   BufferItem *bufferItem = NULL;
   char *address = NULL;

   if (!bufferQueue.isEmpty())
   {
      bufferItem = bufferQueue.dequeue();
      address = bufferItem->getAddress();
      *validFrameCount = bufferItem->getValidFrameCount();
   }
   else
   {
      *validFrameCount = 0;
   }
   return address;
}

int ImageItem::getBufferQueueSize()
{
   QMutexLocker locker(&mutex);
   return bufferQueue.size();
}

