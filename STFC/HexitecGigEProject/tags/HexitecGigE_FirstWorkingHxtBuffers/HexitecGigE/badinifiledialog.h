#ifndef BADINIFILEDIALOG_H
#define BADINIFILEDIALOG_H

#include <QDialog>

namespace Ui {
class BadIniFileDialog;
}

class BadIniFileDialog : public QDialog
{
   Q_OBJECT

public:
   explicit BadIniFileDialog(QWidget *parent = 0);
   ~BadIniFileDialog();

private:
   Ui::BadIniFileDialog *ui;
};

#endif // BADINIFILEDIALOG_H
