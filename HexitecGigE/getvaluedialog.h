#ifndef GETVALUEDIALOG_H
#define GETVALUEDIALOG_H

#include <QDialog>

namespace Ui {
class GetValueDialog;
}

class GetValueDialog : public QDialog
{
   Q_OBJECT
   
public:
   static GetValueDialog *instance();
   ~GetValueDialog();
   void setLabel(QString label);
   double getDoubleValue();
   QString getStringValue();

private:
   static GetValueDialog *gvdInstance;
   GetValueDialog(QWidget *parent = 0);
   Ui::GetValueDialog *ui;
};

#endif // GETVALUEDIALOG_H
