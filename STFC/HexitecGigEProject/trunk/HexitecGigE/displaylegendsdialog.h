#ifndef DISPLAYLEGENDSDIALOG_H
#define DISPLAYLEGENDSDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QTreeWidgetItem>

namespace Ui {
class DisplayLegendsDialog;
}

class DisplayLegendsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DisplayLegendsDialog(QWidget *parent = 0);
    ~DisplayLegendsDialog();
    void update(QString imageName, QPoint pixel, QColor color);    
    void addLegend(QString imageName, QPoint pixel, QColor color);
    void clearLegends();

private:
    Ui::DisplayLegendsDialog *ui;
    QTreeWidgetItem *childItem;
    QTreeWidgetItem *topLevelItem;
};

#endif // DISPLAYLEGENDSDIALOG_H
