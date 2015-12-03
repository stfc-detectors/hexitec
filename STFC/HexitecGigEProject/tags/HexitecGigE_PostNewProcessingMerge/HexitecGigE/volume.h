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
   int sliceCount();
   void removeSlice(Slice *slice);
   Slice *sliceAt(int index);
};

#endif // VOLUME_H
