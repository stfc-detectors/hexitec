#ifndef DPWARNINGDIALOG_H
#define DPWARNINGDIALOG_H

#include <QDialog>

namespace Ui {
class DPWarningDialog;
}

class DPWarningDialog : public QDialog
{
   Q_OBJECT
   
public:
   explicit DPWarningDialog(QWidget *parent = 0);
   ~DPWarningDialog();
   
private:
   Ui::DPWarningDialog *ui;
};

#endif // DPWARNINGDIALOG_H
