#include "mainwindow.h"
#include "daqchoice.h"
#include "matlabchoice.h"
#include "motorchoice.h"
#include "hexitecgigestring.h"

//#include <QtGui>
#include <QStatusBar>
#include <QGridLayout>
#include <QAction>
#include <QMenuBar>

#include <iostream>
using namespace std;

QSettings *settings;
DAQChoice *daqChoice;
MatlabChoice *matlabChoice;
MotorChoice *motor1, *motor2, *motor3, *motor4, *motor5, *motor6, *motor7, *motor8;
HexitecGigEString *aspectIni, *twoEasyIni, *newportXpsIP, *newportXpsPort;

MainWindow::MainWindow(QWidget *parent) :
   QMainWindow(parent)
{
   createMenus();
   settings = new QSettings(QSettings::UserScope, "TEDDI", "HexitecGigE");
   setWindowTitle(QString::fromUtf8("HexitecGigE Configuration V1.0"));
   statusBar()->showMessage(tr("Ready"));
   statusBar()->setStatusTip("Display status tips");

   QWidget *mainWidget = new QWidget();
   QGridLayout *grid = new QGridLayout(parent);
   mainWidget->setLayout(grid);

   grid->addWidget(daqChoice = new DAQChoice(settings), 0, 0);
   grid->addWidget(matlabChoice = new MatlabChoice(settings), 1, 0);
   grid->addWidget(motor1 = new MotorChoice("ssx", settings), 2, 0);
   grid->addWidget(motor2 = new MotorChoice("ssy", settings), 3, 0);
   grid->addWidget(motor3 = new MotorChoice("ssz", settings), 4, 0);
   grid->addWidget(motor4 = new MotorChoice("ssrot", settings), 5, 0);
   grid->addWidget(motor5 = new MotorChoice("galx", settings), 6, 0);
   grid->addWidget(motor6 = new MotorChoice("galy", settings), 7, 0);
   grid->addWidget(motor7 = new MotorChoice("galz", settings), 8, 0);
   grid->addWidget(motor8 = new MotorChoice("galrot", settings), 9, 0);
   grid->addWidget(aspectIni = new HexitecGigEString("aspectIniFilename", settings));
   grid->addWidget(twoEasyIni = new HexitecGigEString("hexitecGigEIniFilename", settings));
   grid->addWidget(newportXpsIP = new HexitecGigEString("newportXPSIPaddress", settings));
   grid->addWidget(newportXpsPort = new HexitecGigEString("newportXPSPort", settings));

   setCentralWidget(mainWidget);
}

void MainWindow::createMenus()
{
   QMenu *fileMenu, *settingsMenu;
   QAction *saveAct, *deleteAct, *quitAct;

   saveAct = new QAction("&Save", this);
   saveAct->setStatusTip("Save HexitecGigE settings as defined");
   connect(saveAct, SIGNAL(triggered()), this, SLOT(accept()));

   deleteAct = new QAction("&Delete", this);
   deleteAct->setStatusTip("Delete HexitecGigE settings");
   connect(deleteAct, SIGNAL(triggered()), this, SLOT(deleteSettings()));

   quitAct = new QAction("&Quit", this);
   quitAct->setStatusTip("Quit HexitecGigE configuration");
   connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

   fileMenu = menuBar()->addMenu("&File");
   settingsMenu = menuBar()->addMenu("&Settings");

   fileMenu->addAction(quitAct);
   settingsMenu->addAction(saveAct);
   settingsMenu->addAction(deleteAct);
}

void MainWindow::accept()
{
   daqChoice->updateSettings();
   matlabChoice->updateSettings();
   motor1->updateSettings();
   motor2->updateSettings();
   motor3->updateSettings();
   motor4->updateSettings();
   motor5->updateSettings();
   motor6->updateSettings();
   motor7->updateSettings();
   motor8->updateSettings();
   aspectIni->updateSettings();
   twoEasyIni->updateSettings();
   newportXpsIP->updateSettings();
   newportXpsPort->updateSettings();
   printStates();
}

void MainWindow::printStates()
{
   cout << "DAQ choice set to " << daqChoice->on() << endl;
   cout << "MATLAB choice set to " << matlabChoice->on() << endl;
   cout << "Motor " << motor1->getMotor().toStdString() << " type = " << motor1->motorType() << endl;
   cout << "Motor " << motor2->getMotor().toStdString() << " type = " << motor2->motorType() << endl;
   cout << "Motor " << motor3->getMotor().toStdString() << " type = " << motor3->motorType() << endl;
   cout << "Motor " << motor4->getMotor().toStdString() << " type = " << motor4->motorType() << endl;
   cout << "Motor " << motor5->getMotor().toStdString() << " type = " << motor5->motorType() << endl;
   cout << "Motor " << motor6->getMotor().toStdString() << " type = " << motor6->motorType() << endl;
   cout << "Motor " << motor7->getMotor().toStdString() << " type = " << motor7->motorType() << endl;
   cout << "Motor " << motor8->getMotor().toStdString() << " type = " << motor8->motorType() << endl;
   cout << "aSpect ini filename " << aspectIni->getText().toStdString() << endl;
   cout << "HexitecGigE ini filename " << twoEasyIni->getText().toStdString() << endl;
   cout << "Newport XPS IP address " << newportXpsIP->getText().toStdString() << endl;
   cout << "Newport XPS IP port " << newportXpsPort->getText().toStdString() << endl;
}

void MainWindow::reject()
{
   exit(0);
}

void MainWindow::deleteSettings()
{
   daqChoice->removeSettings();
   matlabChoice->removeSettings();
   motor1->removeSettings();
   motor2->removeSettings();
   motor3->removeSettings();
   motor4->removeSettings();
   motor5->removeSettings();
   motor6->removeSettings();
   motor7->removeSettings();
   motor8->removeSettings();
   aspectIni->removeSettings();
   twoEasyIni->removeSettings();
   newportXpsIP->removeSettings();
   newportXpsPort->removeSettings();
}
