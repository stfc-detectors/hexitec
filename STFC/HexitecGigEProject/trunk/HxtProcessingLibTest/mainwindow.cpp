#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "processingForm.h"
#include "processingbuffergenerator.h"
#include <ostream>

MainWindow::MainWindow(QWidget *parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow)
{
   ProcessingForm *processingForm;
   ProcessingBufferGenerator *processingBufferGenerator;
   ProcessingDefinition *processingDefinition;

   processingForm = new ProcessingForm();

   ui->setupUi(this);
   QTabWidget *tabs = new QTabWidget(this);
   setCentralWidget(tabs);
   tabs->addTab(processingForm->getMainWindow(), QString("Processing"));

   processingDefinition = new ProcessingDefinition(processingForm->getFrameSize());
   processingBufferGenerator = new ProcessingBufferGenerator(processingDefinition);

   connect(processingForm, SIGNAL(processImages(int, int)),
           processingBufferGenerator, SLOT(handlePostProcessImages(int, int)));
   connect(processingForm, SIGNAL(configureProcessing(int, int, long long)),
           processingBufferGenerator, SLOT(handleConfigureProcessing(int, int, long long)));
   connect(processingForm, SIGNAL(configureProcessing(bool, bool, int, int, QString)),
           processingBufferGenerator, SLOT(handleConfigureProcessing(bool, bool, int, int, QString)));
   connect(processingForm, SIGNAL(configureProcessing(bool, bool, long long, long long, double, bool, QString, QString)),
           processingBufferGenerator, SLOT(handleConfigureProcessing(bool, bool, long long, long long, double, bool, QString, QString)));
   connect(processingForm, SIGNAL(configureProcessing(int, int)),
           processingBufferGenerator, SLOT(handleConfigureProcessing(int, int)));
   connect(processingForm, SIGNAL(configureProcessing(QStringList, QString, QString)),
           processingBufferGenerator, SLOT(handleConfigureProcessing(QStringList, QString, QString)));

}

MainWindow::~MainWindow()
{
   delete ui;
}
