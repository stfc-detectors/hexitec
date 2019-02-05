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


void ProgressForm::handleNewDataAcquisitionState(QString state)
{
    ui->state->setText(state);
}

void ProgressForm::handleRunningAverageEvents(unsigned long averageValue)
{
   ui->runningAverageValue->setText(QString::number(averageValue) + " Events/Frame");
}

void ProgressForm::handleNewDataAcquisitionProgressBarValue(int progressBarValue)
{
    ui->progressBar->setValue(progressBarValue);
}

void ProgressForm::handleNewDataAcquisitionImageProgressValue(int imageProgressValue)
{
    ui->imageProgressBar->setValue(imageProgressValue);
}
