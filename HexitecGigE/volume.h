#ifndef VOLUME_H
#define VOLUME_H
#include <QVector>
#include <QVariant>
#include <QString>

#include "treeitem.h"

class Slice;

class Volume : public TreeItem
{
private:
   QVector<Slice *> *slices;
public:
   Volume(QString name);
   void addSlice(Slice *slice);
   void removeSlice(Slice *slice);
   void replaceSlice(int index, Slice *slice);
   int sliceCount();
   Slice *sliceAt(int index);
};

#endif // VOLUME_H
