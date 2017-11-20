#include "getvaluedialog.h"
#include "ui_getvaluedialog.h"

GetValueDialog *GetValueDialog::gvdInstance = 0;

GetValueDialog *GetValueDialog::instance()
{
   if (gvdInstance == 0)
   {
      gvdInstance = new GetValueDialog();
   }
   return gvdInstance;
}

GetValueDialog::GetValueDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::GetValueDialog)
{
   ui->setupUi(this);
}

GetValueDialog::~GetValueDialog()
{
   delete ui;
}

void GetValueDialog::setLabel(QString label)
{
   ui->label->setText(label);
}

double GetValueDialog::getDoubleValue()
{
   return getStringValue().toDouble();
}

QString GetValueDialog::getStringValue()
{
   return ui->plainTextEdit->toPlainText();
}
