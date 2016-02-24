#include "dpwarningdialog.h"
#include "ui_dpwarningdialog.h"

DPWarningDialog::DPWarningDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::DPWarningDialog)
{
   ui->setupUi(this);
}

DPWarningDialog::~DPWarningDialog()
{
   delete ui;
}
