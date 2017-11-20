#include "matlabvariable.h"

/*
  Constructs from a string as returned by the Matlab 'whos' command.
  */
MatlabVariable::MatlabVariable(QString whosString)
{
   QStringList bits = whosString.trimmed().split(QRegExp("\\s+"));
   if (DataModel::instance()->getItem(bits.at(0)) == NULL)
   {
      data = NULL;
      this->name = bits.at(0);
      setObjectName(bits.at(0));
      QVector<QVariant> data;

      QStringList sizes = bits.at(1).split('x');
      xSize = sizes.at(0).toInt();
      ySize = sizes.at(1).toInt();

      data << objectName() << "Matlab " + bits.at(1) + " " + bits.at(3) << "" << "";
      TreeItem::init(data, new QModelIndex(), objectName(), TreeItem::MATLABVAR);
   }
   else
   {
      throw QString("Already exists");
   }
}

bool MatlabVariable::isArray()
{
   return (xSize > 1 && ySize > 1);
}

/*
  Matlab variables have no data unless it is explicitly imported with this method.
  */
bool MatlabVariable::importData()
{
   data = matlab::instance()->getArray(name);
   //addParameter("Data", "...");
   return (data != NULL);
}

double *MatlabVariable::getData()
{
   return data;
}

int MatlabVariable::getXSize()
{
   return xSize;
}

int MatlabVariable::getYSize()
{
   return ySize;
}

QString MatlabVariable::getName()
{
   return name;
}
