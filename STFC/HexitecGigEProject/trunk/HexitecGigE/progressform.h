#ifndef PROGRESSFORM_H
#define PROGRESSFORM_H

#include <QWidget>
#include <QMainWindow>
#include <QTime>

namespace Ui {
class ProgressForm;
}
const int secs_in_day = 86400;

class ProgressForm : public QWidget
{
   Q_OBJECT

public:
   explicit ProgressForm(QWidget *parent = 0);
   ~ProgressForm();
   QMainWindow *getMainWindow();

private:
   Ui::ProgressForm *ui;
   QMainWindow *mainWindow;
   QString formattedText;
   double collectionTime;
   double remainingTime;
   double daqCollectionTime;
   QString formattedTime(double daqDuration);
   QString prepend0(QString input, int number);
   void updateTimes();

public slots:
   void handleImageStarting(double daqCollectionTime);
   void handleUpdateProgress(double progressTime);
};

#endif // PROGRESSFORM_H
