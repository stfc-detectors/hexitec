#include "datacollectiondialog.h"
#include "ui_datacollectiondialog.h"

DataCollectionDialog::DataCollectionDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::DataCollectionDialog)
{
   ui->setupUi(this);
}

DataCollectionDialog::~DataCollectionDialog()
{
   delete ui;
}
