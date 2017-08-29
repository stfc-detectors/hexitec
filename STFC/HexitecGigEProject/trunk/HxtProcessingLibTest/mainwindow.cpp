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

//   connect(processingForm, SIGNAL(configureProcessing(bool,const char*,const char*,const char*)),
//           processingBufferGenerator, SLOT(handleConfigureProcessing(bool,const char*,const char*,const char*)));
   connect(processingForm, SIGNAL(configureProcessing(bool, bool, int, int, const char*, const char*, const char*, const char*)),
           processingBufferGenerator, SLOT(handleConfigureProcessing(bool, bool, int, int, const char*, const char*, const char*, const char*)));
   connect(processingForm, SIGNAL(configureProcessing(bool, long long, long long, double, bool, const char *, const char *, const char *)),
           processingBufferGenerator, SLOT(handleConfigureProcessing(bool, long long, long long, double, bool, const char *, const char *, const char *)));
   connect(processingForm, SIGNAL(configureProcessing(int, int, const char*, const char*, const char*)),
           processingBufferGenerator, SLOT(handleConfigureProcessing(int, int, const char*, const char*, const char*)));

   connect(processingForm, SIGNAL(imageStarted(const char*, int, int)),
           processingBufferGenerator, SLOT(handleImageStarted(const char *, int, int)));
   connect(processingForm, SIGNAL(transferBufferReady(char*, unsigned long)),
           processingBufferGenerator, SLOT(handleTransferBufferReady(char *, unsigned long)));
   connect(processingForm, SIGNAL(imageComplete(long long)),
           processingBufferGenerator, SLOT(handleImageComplete(long long)));
}

MainWindow::~MainWindow()
{
   delete ui;
}
