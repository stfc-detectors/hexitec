#ifndef DATACOLLECTIONDIALOG_H
#define DATACOLLECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class DataCollectionDialog;
}

class DataCollectionDialog : public QDialog
{
   Q_OBJECT
   
public:
   explicit DataCollectionDialog(QWidget *parent = 0);
   ~DataCollectionDialog();
   
private:
   Ui::DataCollectionDialog *ui;
};

#endif // DATACOLLECTIONDIALOG_H
