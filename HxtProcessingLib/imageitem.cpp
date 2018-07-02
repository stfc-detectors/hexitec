#include "imageitem.h"
#include <iostream>

using namespace std;

ImageItem::ImageItem(const char *filename)
{
   this->filename = new char[1024];
   strcpy(this->filename, filename);
   lock_guard<mutex> lock(iiMutex);
   this->processedFrameCount = 0;
   bufferItem = NULL;
   // Free filename, memory originally allocated in ProcBuffGen::handlePostProcessImages()
//   delete filename;
}

ImageItem::~ImageItem()
{
   delete filename;
}

void ImageItem::enqueueBuffer(char *address, unsigned long validFrameCount)
{
   lock_guard<mutex> lock(iiMutex);
   bufferQueue.push(new BufferItem(address, validFrameCount));
}

char *ImageItem::getNextBuffer(unsigned long *validFrameCount)
{
   lock_guard<mutex> lock(iiMutex);
   char *address = NULL;

   if (bufferItem != NULL)
   {
      delete bufferItem;
   }

   if (!bufferQueue.empty())
   {
      bufferItem = bufferQueue.front();
      bufferQueue.pop();
      address = bufferItem->getAddress();
      *validFrameCount = bufferItem->getValidFrameCount();
   }
   else
   {
      *validFrameCount = 0;
   }
   return address;
}

size_t ImageItem::getBufferQueueSize()
{
   lock_guard<mutex> lock(iiMutex);
   return bufferQueue.size();
}

char *ImageItem::getFilename()
{
   return filename;
}
