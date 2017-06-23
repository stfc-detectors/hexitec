#include "imageitem.h"
#include <QDebug>
#include <QThread>

ImageItem::ImageItem(const char *name, int frameSize)
{
   this->name = (char *)name;
   this->frameSize = frameSize;
   this->bufferQueue.clear();
   this->processedFrameCount = 0;
}

void ImageItem::enqueueBuffer(char *address, unsigned long validFrameCount)
{
   bufferQueue.enqueue(new BufferItem(address, validFrameCount));
   qDebug() << "ImageItem thread = " << QThread::currentThreadId();
   qDebug() << "BUFFER QUEUED";
}

char *ImageItem::getNextBuffer(unsigned long *validFrameCount)
{
   BufferItem *bufferItem = NULL;
   char *address = NULL;
   qDebug() << "++++++++++getting next buffer";

   if (!bufferQueue.isEmpty())
   {
      bufferItem = bufferQueue.dequeue();
      qDebug() << "BUFFER RETURNED";
      address = bufferItem->getAddress();
      *validFrameCount = bufferItem->getValidFrameCount();
   }
   else
   {
      *validFrameCount = 0;
//      qDebug() << "BUFFER QUEUE EMPTY";
   }
   return address;
}

int ImageItem::getBufferQueueSize()
{
   return bufferQueue.size();
}

