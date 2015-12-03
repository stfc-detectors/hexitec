#ifndef OFFSETSDIALOG_H
#define OFFSETSDIALOG_H

#include <QDialog>

namespace Ui {
class OffsetsDialog;
}

class OffsetsDialog : public QDialog
{
   Q_OBJECT
   
public:
   explicit OffsetsDialog(QWidget *parent = 0);
   ~OffsetsDialog();
   
private:
   Ui::OffsetsDialog *ui;
};

#endif // OFFSETSDIALOG_H
