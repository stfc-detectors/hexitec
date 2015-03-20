#include "keithleymainwindow.h"
#include "ui_keithleymainwindow.h"

KeithleyMainWindow::KeithleyMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::KeithleyMainWindow)
{
    ui->setupUi(this);
    plotter = new Plotter(this);
    this->setCentralWidget(plotter);
//    visualisation->setCentralWidget(MainViewer::instance());
//    addToolBar(Qt::TopToolBarArea, MainViewer::instance()->createToolbar());

//    plotter->show();
//    plotter->update();
}

KeithleyMainWindow::~KeithleyMainWindow()
{
    delete ui;
}
