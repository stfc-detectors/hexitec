#include "gridsizequery.h"
#include "ui_gridsizequery.h"

gridSizeQuery::gridSizeQuery(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gridSizeQuery)
{
    ui->setupUi(this);
}

gridSizeQuery::~gridSizeQuery()
{
    delete ui;
}

void gridSizeQuery::setSize(int x, int y)
{
    if (x >= ui->spinBoxRows->minimum() && x <= ui->spinBoxRows->maximum())
        ui->spinBoxRows->setValue(x);
    if (y >= ui->spinBoxColumns->minimum() && x <= ui->spinBoxColumns->maximum())
        ui->spinBoxColumns->setValue(y);
}

void gridSizeQuery::on_okButton_clicked()
{
    numberOfRows = ui->spinBoxRows->value();
    numberOfColumns = ui->spinBoxColumns->value();
    this->close();
}
