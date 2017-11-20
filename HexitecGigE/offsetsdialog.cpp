#include "offsetsdialog.h"
#include "ui_offsetsdialog.h"

OffsetsDialog::OffsetsDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::OffsetsDialog)
{
   ui->setupUi(this);
}

OffsetsDialog::~OffsetsDialog()
{
   delete ui;
}
