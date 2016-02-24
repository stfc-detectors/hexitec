#include "vboutofrangedialog.h"
#include "ui_vboutofrangedialog.h"

VBOutOfRangeDialog::VBOutOfRangeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VBOutOfRangeDialog)
{
    ui->setupUi(this);
}

VBOutOfRangeDialog::~VBOutOfRangeDialog()
{
    delete ui;
}
