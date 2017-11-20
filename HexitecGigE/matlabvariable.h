#ifndef MATLABVARIABLE_H
#define MATLABVARIABLE_H

#include <QString>
#include <QStringList>
#include <QObject>
#include "matlab.h"
#include "treeitem.h"
#include "datamodel.h"

class MatlabVariable : public QObject, public TreeItem
{
   Q_OBJECT

public:
   MatlabVariable(QString whosString);
   bool isArray();
   bool importData();
   double *getData();
   int getXSize();
   int getYSize();
   QString getName();

private:
   QString name;
   int xSize;
   int ySize;
   double *data;
};

#endif // MATLABVARIABLE_H
