#include "badinifiledialog.h"
#include "ui_badinifiledialog.h"

BadIniFileDialog::BadIniFileDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::BadIniFileDialog)
{
   ui->setupUi(this);
}

BadIniFileDialog::~BadIniFileDialog()
{
   delete ui;
}
