#ifndef PROGRESSFORM_H
#define PROGRESSFORM_H

#include <QMainWindow>
#include <QWidget>
#include <QTime>

namespace Ui {
class ProgressForm;
}
const int s_in_day = 86400;

class ProgressForm : public QWidget
{
   Q_OBJECT

public:
   explicit ProgressForm(QWidget *parent = 0);
   ~ProgressForm();
   QMainWindow *getMainWindow();

   void setDaqCollectionTime(double value) {daqCollectionTime = value; }
private:
   Ui::ProgressForm *ui;
   QMainWindow *mainWindow;
   double daqCollectionTime;

public slots:
   void handleNewDataAcquisitionState(QString state);
   void handleNewDataAcquisitionProgressBarValue(int progressBarValue);
   void handleNewDataAcquisitionImageProgressValue(int imageProgressValue);
};

#endif // PROGRESSFORM_H
