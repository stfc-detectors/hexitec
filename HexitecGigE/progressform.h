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
//   QString formattedText;
   double daqCollectionTime;
//   double collectionTime;
//   double remainingTime;
//   QString formattedTime(double daqDuration);
//   QString prepend0(QString input, int number);
//   void updateTimes();

public slots:
//   void handleImageStarting(double daqCollectionTime, int repeatCount, int nRepeat);
//   void handleUpdateProgress(double progressTime);

   void handleNewDataAcquisitionState(QString state);
   void handleNewDataAcquisitionProgressBarValue(int progressBarValue);
   void handleNewDataAcquisitionImageProgressValue(int imageProgressValue);
};

#endif // PROGRESSFORM_H
