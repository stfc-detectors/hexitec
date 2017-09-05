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

   processingDefinition = new ProcessingDefinition(6400);
   processingBufferGenerator = new ProcessingBufferGenerator(processingDefinition);

   connect(processingForm, SIGNAL(processImages()),
           processingBufferGenerator, SLOT(handleProcessImages()));
   connect(processingForm, SIGNAL(configureProcessing(bool, bool, int, int, const char*)),
           processingBufferGenerator, SLOT(handleConfigureProcessing(bool, bool, int, int, const char*)));
   connect(processingForm, SIGNAL(configureProcessing(bool, long long, long long, double, bool, char *, char *, const char *)),
           processingBufferGenerator, SLOT(handleConfigureProcessing(bool, long long, long long, double, bool, char *, char *, const char *)));
   connect(processingForm, SIGNAL(configureProcessing(int, int)),
           processingBufferGenerator, SLOT(handleConfigureProcessing(int, int)));
   connect(processingForm, SIGNAL(configureProcessing(QStringList, QString, QString)),
           processingBufferGenerator, SLOT(handleConfigureProcessing(QStringList, QString, QString)));

}

MainWindow::~MainWindow()
{
   delete ui;
}
