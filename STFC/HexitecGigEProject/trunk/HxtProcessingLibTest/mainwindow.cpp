#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "processingForm.h"
#include "hxtgenerator.h"
#include <ostream>

MainWindow::MainWindow(QWidget *parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow)
{
   ProcessingForm *processingForm;
   HxtGenerator *hxtGenerator;
   ProcessingDefinition *processingDefinition;

   processingForm = new ProcessingForm();

   ui->setupUi(this);
   QTabWidget *tabs = new QTabWidget(this);
   setCentralWidget(tabs);
   tabs->addTab(processingForm->getMainWindow(), QString("Processing"));

   processingDefinition = new ProcessingDefinition();
   hxtGenerator = new HxtGenerator(processingDefinition);

   connect(processingForm, SIGNAL(configureProcessing(int,int,uint16_t*,const char*,const char*,const char*)),
           hxtGenerator, SLOT(handleConfigureProcessing(int,int,uint16_t*,const char*,const char*,const char*)));
   connect(processingForm, SIGNAL(imageStarted(const char*, int)),
           hxtGenerator, SLOT(handleImageStarted(const char *, int)));
   connect(processingForm, SIGNAL(transferBufferReady(char*, unsigned long)),
           hxtGenerator, SLOT(handleTransferBufferReady(char *, unsigned long)));
   connect(processingForm, SIGNAL(imageComplete(unsigned long long)),
           hxtGenerator, SLOT(handleImageComplete(unsigned long long)));
}

MainWindow::~MainWindow()
{
   delete ui;
}
