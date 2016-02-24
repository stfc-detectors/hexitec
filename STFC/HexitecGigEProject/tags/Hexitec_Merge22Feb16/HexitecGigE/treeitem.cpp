#include <QStringList>

#include "treeitem.h"
#include "datamodel.h"
#include "slice.h"

TreeItem::TreeItem(QString name, ItemType type)
{
   QVector<QVariant> rootData;
   rootData << "Name" << "Type" << "Parameter" << "Value";
   this->name = name;
   this->type = type;
   itemData = rootData;
}

TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem *parent, QString name, ItemType type)
{
   this->name = name;
   this->type = type;
   parentItem = parent;
   itemData = data;
}

void TreeItem::init(const QVector<QVariant> &data, QModelIndex *parentIndex, QString name, TreeItem::ItemType type)
{
   DataModel *dataModel = DataModel::instance();
   TreeItem *childItem;
   Slice *slice;
   QString fileName;

   this->name = name;
   this->type = type;
   parentItem = dataModel->getItem(*parentIndex);
   itemData = data;

   if (type == ItemType::SLICE)
   {
      int childCount = parentItem->childCount();
      for  (int i = 0; i < childCount; i++)
      {
         childItem = parentItem->child(i);
         if (childItem->getType() == ItemType::SLICE)
         {
            slice = dataModel->getSlice(parentItem->getName()+"."+childItem->getName());
            fileName = slice->getFileName();
         }
      }
   }

   dataModel->beginInsertRows(parentIndex->parent(), parentIndex->row()+1, parentIndex->row()+1);
   parentItem->appendChild(this);
   dataModel->endInsertRows();
}

int TreeItem::init(const QVector<QVariant> &data, QModelIndex *parentIndex, QString name, QString fileName)
{
   DataModel *dataModel = DataModel::instance();
   TreeItem *childItem;
   Slice *slice;
   QString sliceName;
   int sliceNumber = -1;
   int i = 0;
   int sliceToReplace = -1;
   bool replaceSlice = false;

   this->name = name;
   this->type = ItemType::SLICE;
   parentItem = dataModel->getItem(*parentIndex);
   itemData = data;

   int childCount = parentItem->childCount();
   for  (i = 0; i < childCount; i++)
   {
      childItem = parentItem->child(i);
      if (childItem->getType() == ItemType::SLICE)
      {
         sliceNumber++;
         sliceName = childItem->getName();
         slice = dataModel->getSlice(parentItem->getName() + "." + sliceName);
         if (fileName == slice->getFileName())
         {
            replaceSlice = true;
            this->name = sliceName;
            break;
         }
      }
   }

   if (replaceSlice)
   {
      sliceToReplace = sliceNumber;
      parentItem->replaceChild(i, this);
      slice = dataModel->getSlice(dataModel->index(i, 0, *parentIndex));
      setData(0, sliceName);
      slice->setObjectName(sliceName);
   }
   else
   {
      dataModel->beginInsertRows(parentIndex->parent(), parentIndex->row()+1, parentIndex->row()+1);
      parentItem->appendChild(this);
      dataModel->endInsertRows();
   }
   return sliceToReplace;
}

TreeItem::~TreeItem()
{
   qDeleteAll(childItems);
}

QString TreeItem::getName()
{
   return name;
}

TreeItem::ItemType TreeItem::getType()
{
   return type;
}

TreeItem *TreeItem::child(int number)
{
   return childItems.value(number);
}

TreeItem *TreeItem::child(QString requiredName)
{
   TreeItem *found = NULL;
   QString searchString;

   if (requiredName != NULL)
   {
      searchString = splitRequiredName(requiredName);

      for (int i = 0; i < childItems.size(); i++)
      {
         if ((child(i)->getName()) == searchString)
         {
            if (requiredName == "")
            {
               found = child(i);
            }
            else
            {
               found = child(i)->child(requiredName);
            }
            break;
         }
      }
   }

   return found;
}

QString TreeItem::splitRequiredName(QString &requiredName)
{
   QString searchString = NULL;
   int index = requiredName.indexOf('.');

   if (index != -1)
   {
      searchString = requiredName.left(index);
      requiredName = requiredName.remove(0, index + 1);
   }
   else
   {
      searchString = requiredName;
      requiredName = "";
   }

   return searchString;
}


int TreeItem::childCount() const
{
   return childItems.count();
}

int TreeItem::childNumber() const
{
   if (parentItem)
      return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

   return 0;
}

int TreeItem::columnCount() const
{
   return itemData.count();
}

QVariant TreeItem::data(int column) const
{
   return itemData.value(column);
}

bool TreeItem::insertChildren(int position, int count, int columns)
{
   if (position < 0 || position > childItems.size())
      return false;

   for (int row = 0; row < count; ++row) {
      QVector<QVariant> data(columns);
      TreeItem *item = new TreeItem(data, this);
      childItems.insert(position, item);
   }

   return true;
}

void TreeItem::appendChild(TreeItem *item)
{
   childItems.append(item);
}

void TreeItem::replaceChild(int childNumber, TreeItem *item)
{
   childItems.replace(childNumber, item);
}

void TreeItem::addParameter(QString name, int value)
{
   QVector<QVariant> parameterData;
   parameterData << "" << "" << name << value;
   appendChild(new TreeItem(parameterData, this, name));
}

void TreeItem::addParameter(QString name, QString value)
{
   QVector<QVariant> parameterData;
   parameterData << "" << "" << name << value;
   appendChild(new TreeItem(parameterData, this, name));
}

void TreeItem::setParameter(QString name, QVariant value)
{
   TreeItem *item = child(name);
   item->setData(3, value);
}

bool TreeItem::insertColumns(int position, int columns)
{
   if (position < 0 || position > itemData.size())
      return false;

   for (int column = 0; column < columns; ++column)
      itemData.insert(position, QVariant());

   foreach (TreeItem *child, childItems)
      child->insertColumns(position, columns);

   return true;
}

TreeItem *TreeItem::parent()
{
   return parentItem;
}

bool TreeItem::removeChildren(int position, int count)
{
   if (position < 0 || position + count > childItems.size())
      return false;

   for (int row = 0; row < count; ++row)
      delete childItems.takeAt(position);

   return true;
}

bool TreeItem::removeColumns(int position, int columns)
{
   if (position < 0 || position + columns > itemData.size())
      return false;

   for (int column = 0; column < columns; ++column)
      itemData.remove(position);

   foreach (TreeItem *child, childItems)
      child->removeColumns(position, columns);

   return true;
}

bool TreeItem::setData(int column, const QVariant &value)
{
   if (column < 0 || column >= itemData.size())
      return false;

   itemData[column] = value;
   return true;
}
