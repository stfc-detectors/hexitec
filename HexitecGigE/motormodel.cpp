#include "motormodel.h"


MotorModel *MotorModel::motorModelHandle(0);

MotorModel::MotorModel(QObject *parent)
   :QAbstractTableModel(parent)
{
   MotorModel::motorModelHandle = this;
}

MotorModel::~MotorModel()
{
   while (!motors.isEmpty())
   {
      delete motors.takeFirst();
   }
}

MotorModel *MotorModel::getMotorModelHandle()
{
    return motorModelHandle;
}

void MotorModel::setColumnHeaderData(QList<QString> headers)
{
   this->headers = headers;
   nameColumn = headers.indexOf("Name");
   statusColumn = headers.indexOf("Status");
   positionColumn = headers.indexOf("Position");
   moveColumn = headers.indexOf("Move");
   modeColumn = headers.indexOf("Mode");
   startColumn = headers.indexOf("Start");
}

QVariant MotorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   QVariant qv = QVariant();

   if (role == Qt::DisplayRole)
   {
      if (orientation == Qt::Horizontal)
      {
         qv = QString(headers.at(section));
      }
   }

   return qv;
}

QList <Motor *> MotorModel::getMotors()
{
   return motors;
}

void MotorModel::addMotor(Motor *motor)
{
   motors.append(motor);
   connect(motor, SIGNAL(positionChanged(Motor *, const QVariant &)), this,
           SLOT(positionChanged(Motor *, const QVariant &)));
   connect(motor, SIGNAL(statusChanged(Motor *, MotorStatus)), this,
           SLOT(statusChanged(Motor *, MotorStatus)));
   connect(motor, SIGNAL(moveInputChanged(Motor *, const QVariant &)), this,
           SLOT(moveInputChanged(Motor *, const QVariant &)));
   connect(motor, SIGNAL(modeChanged(Motor *, const QVariant &)), this,
           SLOT(modeChanged(Motor *, const QVariant &)));
}

int MotorModel::rowCount(const QModelIndex & /*parent*/) const
{
   return motors.count();
}

int MotorModel::columnCount(const QModelIndex & /*parent*/) const
{
   return COLS;
}

QVariant MotorModel::data(const QModelIndex &index, int role) const
{
   QVariant result = QVariant();
   Motor *motor;
   int column = index.column();

   if (role == Qt::DisplayRole)
   {
      motor = motors.at(index.row());

      if (column == nameColumn)
      {
         result = QVariant(motor->property("objectName"));
      }
      else if (column == statusColumn)
      {
         result = QVariant(motor->getStatusString());
      }
      else if (column == positionColumn)
      {
         result = QVariant(motor->getPosition());
      }
      else if (column ==  moveColumn)
      {
         result = QVariant(motor->getMoveInput());
      }
      else if (column ==  modeColumn)
      {
         result = QVariant(motor->getMode());
      }
   }

   return result;
}

bool MotorModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
   int column = index.column();

   if (role == Qt::EditRole)
   {
      if (column == positionColumn)
      {
         /* We don't need to do anything other than emit dataChanged() below.
           Any views interested in the model data will access the motor directly
           through the data() method.
           */
      }
      else if (column == statusColumn)
      {
         /* We don't need to do anything other than emit dataChanged() below.
           Any views interested in the model data will access the motor directly
           through the data() method.
           */
      }
      else if (column == moveColumn)
      {
         /* setMoveInput is called with false so that no signal is sent and no infinte loop started*/
         motors.at(index.row())->setMoveInput(value.toInt(), false);
      }
      else if (column == modeColumn)
      {
         /* setMode is called with false so that no signal is sent and no infinte loop started*/
         motors.at(index.row())->setMode(value.toInt(), false);
      }
      else if (column == startColumn)
      {
         if (value.toInt() >= 0)
         {
            motors.at(index.row())->setSelected(value.toInt());
         }
      }
      emit dataChanged(index, index);
   }
   return true;
}

void MotorModel::positionChanged(Motor *motor, const QVariant & value)
{
   int row = motors.indexOf(motor);

   if (row >= 0)
   {
      setData(index(row, 2), value);
   }
}

void MotorModel::statusChanged(Motor *motor, MotorStatus motorStatus)
{
   int row = motors.indexOf(motor);

   if (row >= 0)
   {
      setData(index(row, 1), motorStatus.getStringStatus());
   }
}

void MotorModel::moveInputChanged(Motor *motor, const QVariant &)
{
   int row = motors.indexOf(motor);

   if (row >= 0)
   {
      //setData(index(row, moveColumn), value);
      /* Instead of settting the moveInput we now tell the table to get if from the motor */
      emit dataChanged(index(row, moveColumn), index(row, moveColumn));
   }
}

void MotorModel::modeChanged(Motor *motor, const QVariant &)
{
   int row = motors.indexOf(motor);

   if (row >= 0)
   {
      //setData(index(row, modeColumn), value);
      /* Instead of settting the mode we now tell the table to get if from the motor */
      emit dataChanged(index(row, modeColumn), index(row, modeColumn));
   }
}

Qt::ItemFlags MotorModel::flags(const QModelIndex & index) const
{
   int column = index.column();
   Qt::ItemFlags flags = QAbstractItemModel::flags(index);

   if (column != nameColumn && column != statusColumn && column != positionColumn)
   {
      flags |= Qt::ItemIsEditable;
   }

   return flags;
}

