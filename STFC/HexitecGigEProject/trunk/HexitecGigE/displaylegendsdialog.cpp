#include "displaylegendsdialog.h"
#include "ui_displaylegendsdialog.h"

DisplayLegendsDialog::DisplayLegendsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayLegendsDialog)
{
    ui->setupUi(this);
}

DisplayLegendsDialog::~DisplayLegendsDialog()
{
    delete ui;
}

void DisplayLegendsDialog::addLegend(QString imageName, QPoint pixel, QColor color)
{
    QString text;

    QList <QTreeWidgetItem *> imageList = ui->plotLegends->findItems(imageName, Qt::MatchFixedString);

    if (imageList.isEmpty())
    {
        topLevelItem = new QTreeWidgetItem;
        topLevelItem->setText(0, imageName);
        ui->plotLegends->addTopLevelItem(topLevelItem);
    }
    else
    {
        topLevelItem = imageList.first();
    }
    childItem = new QTreeWidgetItem(topLevelItem);
    text = QString::number(pixel.rx() + 1);
    childItem->setTextColor(1, color);
    childItem->setText(1, text);
    text = QString::number(pixel.ry() + 1);
    childItem->setTextColor(2, color);
    childItem->setText(2, text);
}

void DisplayLegendsDialog::clearLegends()
{
    ui->plotLegends->clear();
}
