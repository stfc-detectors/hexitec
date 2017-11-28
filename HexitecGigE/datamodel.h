#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QVector>

// Putting the .h files for these classes here can introduce circularity problems so
// we just declare the classes and put the .h files in the .cpp file.
class Volume;
class Slice;
//class MatlabVariable;
class TreeItem;

class DataModel : public QAbstractItemModel
{
   Q_OBJECT

public:
   friend class TreeItem;

   DataModel(const QStringList &headers, const QString &data,
             QObject *parent = 0);
   ~DataModel();
   static DataModel *instance();

   QVariant data(const QModelIndex &index, int role) const;
   QVariant headerData(int section, Qt::Orientation orientation,
                       int role = Qt::DisplayRole) const;

   QModelIndex index(int row, int column,
                     const QModelIndex &parent = QModelIndex()) const;
   QModelIndex parent(const QModelIndex &index) const;

   int rowCount(const QModelIndex &parent = QModelIndex()) const;
   int columnCount(const QModelIndex &parent = QModelIndex()) const;

   Qt::ItemFlags flags(const QModelIndex &index) const;
   bool setData(const QModelIndex &index, const QVariant &value,
                int role = Qt::EditRole);
   bool setHeaderData(int section, Qt::Orientation orientation,
                      const QVariant &value, int role = Qt::EditRole);

   bool insertColumns(int position, int columns,
                      const QModelIndex &parent = QModelIndex());
   bool removeColumns(int position, int columns,
                      const QModelIndex &parent = QModelIndex());
   bool insertRows(int position, int rows,
                   const QModelIndex &parent = QModelIndex());
   bool removeRows(int position, int rows,
                   const QModelIndex &parent = QModelIndex());
   TreeItem *getRootItem();

   QModelIndex getItemIndex(QString name);
   TreeItem *getItem(QString name);
   Volume *getVolume(QString name);
   Slice *getSlice(QString name);
   Slice *getSlice(const QModelIndex &index);
//   MatlabVariable *getMatlabArray(QModelIndex &index);
//   void removeAllMatlabVariables();
   void removeSlice(const QModelIndex &index);
   void add(QModelIndex index);
   void end();

   // New methods to do with replacing active slice - currently in state of transition
   void deleteSlice(Slice *slice);
   Slice *getActiveSlice();
   Slice *sliceAt(int index);
   void setActiveSlice(Slice *slice);
   int numberOfSlices();
private:
   DataModel(QObject *parent = 0);
   static DataModel *dmInstance;
   TreeItem *getItem(const QModelIndex &index) const;
   TreeItem *rootItem;
   Volume *activeVolume;
   Slice *activeSlice;

public slots:
   void dataChanged1(const QModelIndex &topLeft, const QModelIndex &bottomRight);
};


#endif // DATAMODEL_H
