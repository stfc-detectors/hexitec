#include <QtGui>

#include "datamodel.h"
#include "treeitem.h"
#include "volume.h"
#include "slice.h"
//#include "matlabvariable.h"


DataModel *DataModel::dmInstance = 0;

DataModel *DataModel::instance()
{
   if (dmInstance == 0)
   {
      dmInstance = new DataModel();
      // The creation of activeVolume cannot go in the constructor because creation of its
      // TreeItem needs dmInstance.
      dmInstance->activeVolume = new Volume("myVolume");
      dmInstance->activeSlice = NULL;
   }
   return dmInstance;
}

DataModel::DataModel(QObject *parent)
   : QAbstractItemModel(parent)
{
   rootItem = new TreeItem();
}

DataModel::DataModel(const QStringList &headers, const QString &data,
                     QObject *parent)
   : QAbstractItemModel(parent)
{
   rootItem = new TreeItem();
}

DataModel::~DataModel()
{
   delete rootItem;
}

int DataModel::columnCount(const QModelIndex & /* parent */) const
{
   return rootItem->columnCount();
}

QVariant DataModel::data(const QModelIndex &index, int role) const
{
   if (!index.isValid())
      return QVariant();

   if (role != Qt::DisplayRole && role != Qt::EditRole)
      return QVariant();

   TreeItem *item = getItem(index);

   return item->data(index.column());
}

Qt::ItemFlags DataModel::flags(const QModelIndex &index) const
{
   if (!index.isValid())
      return 0;

   return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

TreeItem *DataModel::getItem(const QModelIndex &index) const
{
   if (index.isValid()) {
      TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
      if (item) return item;
   }
   return rootItem;
}

QVariant DataModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      return rootItem->data(section);

   return QVariant();
}

QModelIndex DataModel::index(int row, int column, const QModelIndex &parent) const
{
   if (parent.isValid() && parent.column() != 0)
      return QModelIndex();

   TreeItem *parentItem = getItem(parent);

   TreeItem *childItem = parentItem->child(row);
   if (childItem)
      return createIndex(row, column, childItem);
   else
      return QModelIndex();
}

bool DataModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
   bool success;

   beginInsertColumns(parent, position, position + columns - 1);
   success = rootItem->insertColumns(position, columns);
   endInsertColumns();

   return success;
}

bool DataModel::insertRows(int position, int rows, const QModelIndex &parent)
{
   TreeItem *parentItem = getItem(parent);
   bool success;

   beginInsertRows(parent, position, position + rows - 1);
   success = parentItem->insertChildren(position, rows, rootItem->columnCount());
   endInsertRows();

   return success;
}

QModelIndex DataModel::parent(const QModelIndex &index) const
{
   if (!index.isValid())
      return QModelIndex();

   TreeItem *childItem = getItem(index);
   TreeItem *parentItem = childItem->parent();

   if (parentItem == rootItem)
      return QModelIndex();

   return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool DataModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
   bool success;

   beginRemoveColumns(parent, position, position + columns - 1);
   success = rootItem->removeColumns(position, columns);
   endRemoveColumns();

   if (rootItem->columnCount() == 0)
      removeRows(0, rowCount());

   return success;
}

bool DataModel::removeRows(int position, int rows, const QModelIndex &parent)
{
   TreeItem *parentItem = getItem(parent);
   bool success = true;

   beginRemoveRows(parent, position, position + rows - 1);
   success = parentItem->removeChildren(position, rows);
   endRemoveRows();

   return success;
}

int DataModel::rowCount(const QModelIndex &parent) const
{
   TreeItem *parentItem = getItem(parent);

   return parentItem->childCount();
}

bool DataModel::setData(const QModelIndex &index, const QVariant &value,
                        int role)
{
   if (role != Qt::EditRole)
      return false;

   TreeItem *item = getItem(index);
   bool result = item->setData(index.column(), value);

   if (result)
      emit dataChanged(index, index);

   return result;
}

bool DataModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
   if (role != Qt::EditRole || orientation != Qt::Horizontal)
      return false;

   bool result = rootItem->setData(section, value);

   if (result)
      emit headerDataChanged(orientation, section, section);

   return result;
}

TreeItem *DataModel::getRootItem()
{
   return rootItem;
}

QModelIndex DataModel::getItemIndex(QString name)
{
   TreeItem *item = getItem(name);
   return createIndex(item->childNumber(), 0, item);
}

TreeItem *DataModel::getItem(QString name)
{
   return rootItem->child(name);
}

Volume *DataModel::getVolume(QString name)
{
   Volume *volume = NULL;
   TreeItem *item = getItem(name);
   if ((item != NULL) && (item->getType() == TreeItem::VOLUME))
   {
      volume = static_cast<Volume *>(item);
   }
   return volume;
}

Slice *DataModel::getSlice(QString name)
{
   Slice *slice = NULL;
   TreeItem *item = getItem(name);
   if ((item != NULL) && (item->getType() == TreeItem::SLICE))
   {
      slice = static_cast<Slice *>(item);
   }
   return slice;
}

Slice *DataModel::getSlice(const QModelIndex &index)
{
   Slice *slice = NULL;
   TreeItem *item = DataModel::instance()->getItem(index);
   if ((item != NULL) && (item->getType() == TreeItem::SLICE))
   {
      slice = static_cast<Slice *>(item);
   }
   return slice;
}

//MatlabVariable *DataModel::getMatlabArray(QModelIndex &index)
//{
//   MatlabVariable *mv = NULL;
//   TreeItem *item = DataModel::instance()->getItem(index);
//   if ((item != NULL) && (item->getType() == TreeItem::MATLABVAR))
//   {
//      mv = static_cast<MatlabVariable *>(item);
//      if (!mv->isArray())
//      {
//         mv = NULL;
//      }
//   }
//   return mv;
//}

//void DataModel::removeAllMatlabVariables()
//{

//}

void DataModel::removeSlice(const QModelIndex &index)
{
   // This detaches the slice from its Volume (if it's got one)
   dmInstance->getSlice(index)->detach();
   // This removes the slice from the dataModel
   dmInstance->removeRow(index.row(), index.parent());
}

void DataModel::dataChanged1(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
   emit dataChanged(topLeft, bottomRight);
}

void DataModel::add(QModelIndex index)
{
   beginInsertRows(index.parent(), index.row()+1, index.row()+2);
}

void DataModel::end()
{
   endInsertRows();
}

/* deleteSlice deals with both the activeSlice and the DataModel aspects of deleting Slice.
*/
void DataModel::deleteSlice(Slice *slice)
{
   if (slice == dmInstance->activeSlice)
   {
      dmInstance->activeSlice = NULL;
   }
   // The method for getting an index from a name requires a fully qualified name.
   dmInstance->removeSlice(dmInstance->getItemIndex(dmInstance->activeVolume->getName() + "." + slice->getName()));
}

Slice *DataModel::getActiveSlice()
{
   return dmInstance->activeSlice;
}

Slice *DataModel::sliceAt(int index)
{
   return dmInstance->activeVolume->sliceAt(index);
}

void DataModel::setActiveSlice(Slice *slice)
{
   activeSlice = slice;
}

int DataModel::numberOfSlices()
{
   return activeVolume->sliceCount();
}
