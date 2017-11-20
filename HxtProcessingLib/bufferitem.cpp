#include "bufferitem.h"
#include <iostream>

using namespace std;

BufferItem::BufferItem(char *address, unsigned long validFrameCount)
{
   this->address = address;
   this->validFrameCount = validFrameCount;
}

char *BufferItem::getAddress() const
{
   return address;
}

void BufferItem::setAddress(char *value)
{
   address = value;
}

unsigned long BufferItem::getValidFrameCount() const
{
   return validFrameCount;
}

void BufferItem::setValidFrameCount(unsigned long value)
{
   validFrameCount = value;
}
