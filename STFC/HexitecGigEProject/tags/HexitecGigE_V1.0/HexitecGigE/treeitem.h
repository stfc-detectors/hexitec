#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QVector>
#include <QModelIndex>

class TreeItem
{
public:
   enum ItemType {NONE, VOLUME, SLICE, MATLABVAR};

   TreeItem(QString name = NULL, ItemType type = NONE);
   TreeItem(const QVector<QVariant> &data, TreeItem *parent = 0, QString name = NULL, ItemType type = NONE);
   // NB TreeItem destructor MUST be virtual because we refer to classes derived from it (e.g. Slice) by
   // TreeItem pointers.
   virtual ~TreeItem();

   TreeItem *child(int number);
   TreeItem *child(QString name);
   int childCount() const;
   int columnCount() const;
   QVariant data(int column) const;
   bool insertChildren(int position, int count, int columns);
   bool insertColumns(int position, int columns);
   TreeItem *parent();
   bool removeChildren(int position, int count);
   bool removeColumns(int position, int columns);
   int childNumber() const;
   bool setData(int column, const QVariant &value);
   void appendChild(TreeItem *item);
   void replaceChild(int childToReplace, TreeItem *item);
   void addParameter(QString name, int value);
   void addParameter(QString name, QString value);
   void setParameter(QString name, QVariant value);
   ItemType getType();
   QString getName();
protected:

   QString name;
   void init(const QVector<QVariant> &data, QModelIndex *parentIndex, QString name, ItemType type = NONE);
   int init(const QVector<QVariant> &data, QModelIndex *parentIndex, QString name, QString fileName);
private:
   QList<TreeItem*> childItems;
   QVector<QVariant> itemData;
   TreeItem *parentItem;

   QString splitRequiredName(QString &requiredName);
   ItemType type;
};

#endif
