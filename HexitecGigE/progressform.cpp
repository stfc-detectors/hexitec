#include "progressform.h"
#include "ui_progressform.h"
#include <QDebug>

ProgressForm::ProgressForm(QWidget *parent) :
   QWidget(parent),
   ui(new Ui::ProgressForm)
{
   ui->setupUi(this);
   mainWindow = new QMainWindow();
   mainWindow->setCentralWidget(this);
}

ProgressForm::~ProgressForm()
{
   delete ui;
}

QMainWindow *ProgressForm::getMainWindow()
{
   return mainWindow;
}

//void ProgressForm::handleImageStarting(double daqCollectionTime, int repeatCount, int nRepeat)
//{
//   this->daqCollectionTime = daqCollectionTime;
//   collectionTime = (daqCollectionTime/(double) nRepeat) * (double)repeatCount;
//   remainingTime = daqCollectionTime - (daqCollectionTime/(double)nRepeat) * (double)repeatCount;

//   ui->daqTimeFormattedLabel->setText(formattedTime(daqCollectionTime));

//   updateTimes();
//}

//void ProgressForm::handleUpdateProgress(double progressTime)
//{
////   qDebug() << "ProgressForm::handleUpdateProgress(double progressTime)" << progressTime;
//   collectionTime += progressTime;
//   remainingTime -= progressTime;

//   if (collectionTime > daqCollectionTime)
//   {
//      collectionTime = daqCollectionTime;
//      remainingTime = 0.0;
//   }

//   updateTimes();
//}

void ProgressForm::handleNewDataAcquisitionState(QString state)
{
    ui->state->setText(state);
}

void ProgressForm::handleNewDataAcquisitionProgressBarValue(int progressBarValue)
{
    ui->progressBar->setValue(progressBarValue);
}

void ProgressForm::handleNewDataAcquisitionImageProgressValue(int imageProgressValue)
{
    ui->imageProgressBar->setValue(imageProgressValue);
}


//void ProgressForm::updateTimes()
//{
//   ui->currentTimeFormattedLabel->setText(formattedTime(collectionTime));
////   ui->currentTimeSecsLabel->setText("Seconds:" + QString::number(collectionTime));

//   ui->remainingTimeFormattedLabel->setText(formattedTime(remainingTime));
////   ui->remainingTimeSecsLabel->setText("Seconds:" + QString::number(remainingTime));
//}

//QString ProgressForm::formattedTime(double daqDuration)
//{
//   QString formattedText;
//   long daqDurationSeconds = floor(daqDuration);
//   long days = daqDurationSeconds / s_in_day;
//   long hundredths = floor((daqDuration - daqDurationSeconds) * 100 + 0.5);
//   if (days > 0)
//   {
//       formattedText = QString::number(days) +"d ";
//   }

//   formattedText += QTime(0,0).addSecs(daqDurationSeconds).toString("hh:mm:ss");;
//   QString hundredthsStr;

//   hundredthsStr = QString::number(hundredths);
//   hundredthsStr = prepend0(hundredthsStr, 2);

//   formattedText += "." +
//         hundredthsStr;
//   return formattedText;
//}

//QString ProgressForm::prepend0(QString input, int number)
//{
//   int leadingZeros;

//   leadingZeros = number - input.length();
//   for (int i = 0; i < leadingZeros; i++)
//   {
//      input.prepend('0');
//   }

//   return input;
//}


