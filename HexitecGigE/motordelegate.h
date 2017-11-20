#ifndef MOTORDELEGATE_H
#define MOTORDELEGATE_H

#include <QItemDelegate>

class MotorDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    MotorDelegate(int modeColumn, int startColumn, QObject *parent = 0);

//    void paint(QPainter *painter, const QStyleOptionViewItem &option,
//               const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

private:
    int modeColumn;
    int startColumn;
};

#endif
