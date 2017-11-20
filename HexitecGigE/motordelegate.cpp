//#include <QtGui>
#include <QComboBox>
#include <QCheckBox>

#include "motordelegate.h"

MotorDelegate::MotorDelegate(int modeColumn, int startColumn, QObject *parent)
   : QItemDelegate(parent)
{
   this->modeColumn = modeColumn;
   this->startColumn = startColumn;
}

QWidget *MotorDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
   if (index.column() == modeColumn)
   {
      QComboBox *modeCombo = new QComboBox(parent);
      QStringList items;
      items << "Absolute" << "Relative";
      modeCombo->addItems(items);
      modeCombo->setCurrentIndex(0);
      return modeCombo;
   }
   else if (index.column() == startColumn)
   {
      QCheckBox *tickCheckBox = new QCheckBox(parent);
      tickCheckBox->setChecked(false);
      return tickCheckBox;
   }
   else
   {
      return QItemDelegate::createEditor(parent, option, index);
   }
}

void MotorDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
   if (index.column() == modeColumn)
   {
      int mode = index.model()->data(index, Qt::DisplayRole).toInt();
      QComboBox *modeCombo = qobject_cast<QComboBox*>(editor);
      modeCombo->setCurrentIndex(mode);
   }
   else if (index.column() == startColumn)
   {
      QCheckBox *tickCheckBox = qobject_cast<QCheckBox*>(editor);
      tickCheckBox->setText("");
   }
   else
   {
      QItemDelegate::setEditorData(editor, index);
   }
}

void MotorDelegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
   if (index.column() == modeColumn)
   {
      QComboBox *modeCombo = qobject_cast<QComboBox *>(editor);
      int mode = modeCombo->currentIndex();
      model->setData(index, mode);
   }
   else if (index.column() == startColumn)
   {
      QCheckBox *tickCheckBox = qobject_cast<QCheckBox *>(editor);
      int tick = tickCheckBox->isChecked();
      model->setData(index, tick);
   }
   else
   {
      QItemDelegate::setModelData(editor, model, index);
   }
}
