#include "volume.h"
#include "volume.h"
#include "datamodel.h"

Volume::Volume(QString name)
{
   this->name = name;
   slices = new QVector<Slice *>();

   QVector<QVariant> volumeData;
   volumeData << name << "Volume" << "" << "";

   TreeItem::init(volumeData, new QModelIndex(), name, TreeItem::VOLUME);
   addParameter("Slice count", 0);

   // These lines would make the Volume available to scripting, it would have to become a QObject too.
   //  setProperty("objectName", name);
   // emit addObject(this);
}


void Volume::addSlice(Slice *slice)
{
   slices->append(slice);
   int i = slices->count();
   setParameter("Slice count", QVariant(slices->count()));
}

void Volume::replaceSlice(int index, Slice *slice)
{
   slices->replace(index, slice);
   int i = slices->count();
   setParameter("Slice count", QVariant(slices->count()));
}

void Volume::removeSlice(Slice *slice)
{
   int i = slices->indexOf(slice);
   slices->remove(i);
   setParameter("Slice count", QVariant(slices->count()));
}

// This allows a slice to be found by its index in the slices array rather than by its index in
// the TreeItem children array.
Slice *Volume::sliceAt(int index)
{
   Slice *slice = NULL;
   if (index < slices->count())
   {
      slice = slices->at(index);
   }
   return slice;
}

// This is called sliceCount to avoid confusion with the TreeItem count method which also
// counts children which are not Slices.
int Volume::sliceCount()
{
   return slices->count();
}

