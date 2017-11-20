#ifndef BUFFERITEM_H
#define BUFFERITEM_H


class BufferItem
{
public:
   BufferItem(char *address, unsigned long validFrameCount);

   char *getAddress() const;
   void setAddress(char *value);
   unsigned long getValidFrameCount() const;
   void setValidFrameCount(unsigned long value);

private:
   char *address;
   unsigned long validFrameCount;
};

#endif // BUFFERITEM_H
