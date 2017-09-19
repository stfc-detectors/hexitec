#include "imageitem.h"
#include <QMutexLocker>
#include <QDebug>
#include <QThread>
#include <iostream>

using namespace std;

ImageItem::ImageItem(const char *filename, int nRows, int nCols)
{
   qDebug() << "Creating an image item with filename: " << filename;
   this->filename = new char[1024];
   strcpy(this->filename, filename);
   QMutexLocker locker(&mutex);
   this->bufferQueue.clear();
   this->processedFrameCount = 0;
   bufferItem = NULL;
}

ImageItem::~ImageItem()
{
   delete filename;
}

void ImageItem::enqueueBuffer(char *address, unsigned long validFrameCount)
{
   QMutexLocker locker(&mutex);
   bufferQueue.enqueue(new BufferItem(address, validFrameCount));
}

char *ImageItem::getNextBuffer(unsigned long *validFrameCount)
{
   QMutexLocker locker(&mutex);
   char *address = NULL;

   if (bufferItem != NULL)
   {
      delete bufferItem;
   }

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

char *ImageItem::getFilename()
{
   return filename;
}
