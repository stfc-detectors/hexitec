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

   processingDefinition = new ProcessingDefinition();
   processingBufferGenerator = new ProcessingBufferGenerator(processingDefinition);

   connect(processingForm, SIGNAL(configureProcessing(bool,const char*,const char*,const char*)),
           processingBufferGenerator, SLOT(handleConfigureProcessing(bool,const char*,const char*,const char*)));
   connect(processingForm, SIGNAL(configureProcessing(int, int, uint16_t*, const char*, const char*, const char*)),
           processingBufferGenerator, SLOT(handleConfigureProcessing(int, int, uint16_t*, const char*, const char*, const char*)));
   connect(processingForm, SIGNAL(configureProcessing(bool, unsigned long long, unsigned long long, unsigned long long, bool, const char *, const char *, const char *)),
           processingBufferGenerator, SLOT(handleConfigureProcessing(bool, unsigned long long, unsigned long long, unsigned long long, bool, const char *, const char *, const char *)));

   connect(processingForm, SIGNAL(imageStarted(const char*, int)),
           processingBufferGenerator, SLOT(handleImageStarted(const char *, int)));
   connect(processingForm, SIGNAL(transferBufferReady(char*, unsigned long)),
           processingBufferGenerator, SLOT(handleTransferBufferReady(char *, unsigned long)));
   connect(processingForm, SIGNAL(imageComplete(unsigned long long)),
           processingBufferGenerator, SLOT(handleImageComplete(unsigned long long)));
}

MainWindow::~MainWindow()
{
   delete ui;
}
