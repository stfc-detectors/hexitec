#ifndef MOTORMODEL_H
#define MOTORMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QString>
#include <QTableWidget>
#include "motor.h"

const int COLS= 6;

class MotorModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   explicit MotorModel(QObject *parent = 0);
   ~MotorModel();
   static MotorModel *motorModelHandle;
   static MotorModel *getMotorModelHandle();
   QList <Motor *> getMotors();
   int rowCount(const QModelIndex &parent = QModelIndex()) const ;
   int columnCount(const QModelIndex &parent = QModelIndex()) const;
   QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
   bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
   Qt::ItemFlags flags(const QModelIndex & index) const ;
   void addMotor(Motor *motor);
   QVariant headerData(int section, Qt::Orientation orientation, int role) const;
   void setColumnHeaderData(QList <QString> headers);
protected:
   QList <Motor *> motors;

private:
   QList<QString> headers;
   int nameColumn;
   int statusColumn;
   int positionColumn;
   int moveColumn;
   int modeColumn;
   int startColumn;

private slots:
   void positionChanged(Motor *motor, const QVariant & value);
   void statusChanged(Motor *motor, MotorStatus motorStatus);
   void moveInputChanged(Motor *motor, const QVariant & value);
   void modeChanged(Motor *motor, const QVariant & value);
};

#endif // MOTORMODEL_H
