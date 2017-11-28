#include "mainviewer.h"

MainViewer *MainViewer::mVInstance = 0;

MainViewer *MainViewer::instance()
{
   if (mVInstance == 0)
   {
      mVInstance = new MainViewer();
   }
   return mVInstance;
}

RenderArea *MainViewer::getRenderArea()
{
   return renderArea;
}

AxisSettings MainViewer::getAxes()
{
   return renderArea->getAxes();
}

QToolBar *MainViewer::createToolbar()
{
   QToolBar *renderToolBar = new QToolBar(tr("Render Area"));
   QAction *renderZoomInAct = new QAction(QIcon(":/images/zoomIn.png"), tr(""),this);
   QAction *renderZoomOutAct = new QAction(QIcon(":/images/zoomOut.png"), tr(""),this);
   QAction *renderGridToggle = new QAction(QIcon(":/images/grid.png"), tr(""),this);
   QAction *colorBarsToggle = new QAction(QIcon(":/images/lockSTAR.png"), tr(""),this);
   QAction *cycleColorMap = new QAction(QIcon(":/images/colorWheel.png"), tr(""),this);

   renderZoomInAct->setText("Zoom In");
   renderZoomOutAct->setText("Zoom Out");
   colorBarsToggle->setText(tr("Toggle Colour Bars"));
   renderGridToggle->setText(tr("Toggle Grid"));

   renderToolBar->addAction(renderZoomInAct);
   renderToolBar->addAction(renderZoomOutAct);
   renderToolBar->addAction(renderGridToggle);
   renderToolBar->addAction(colorBarsToggle);
   renderToolBar->addAction(cycleColorMap);

   connect(renderZoomInAct, SIGNAL(triggered()), renderArea, SLOT(zoomIn()));
   connect(renderZoomOutAct, SIGNAL(triggered()), renderArea, SLOT(zoomOut()));
   connect(renderGridToggle, SIGNAL(triggered()), renderArea, SLOT(gridToggle()));
   connect(colorBarsToggle, SIGNAL(triggered()), renderArea, SLOT(colorBarsToggle()));
   connect(cycleColorMap, SIGNAL(triggered()), renderArea, SLOT(cycleColorMap()));

   return renderToolBar;
}

void MainViewer::showNewActiveSlice()
{
   renderArea->Hexitec_logoOff();
   renderArea->renderSum(DataModel::instance()->getActiveSlice());
   renderArea->title(DataModel::instance()->getActiveSlice()->getTitle());
   renderArea->setDynamicStateOn();
   renderArea->update();
}

//void MainViewer::showMatlabArray(MatlabVariable *ma)
//{
//   renderArea->Hexitec_logoOff();
//   int xSize = ma->getXSize();
//   int ySize = ma->getYSize();
//   double *data = ma->getData();
//   SArray <double> imageData = SArray<double>();
//   imageData.resize(xSize, ySize);
//   for (int i = 0; i < xSize; i++)
//   {
//      for(int j = 0; j < ySize; j++)
//      {
//         imageData[i][j] = data[i * ySize + j];
//      }
//   }
//   renderArea->setImageData(imageData);
//   renderArea->title(ma->getName());
//   renderArea->update();
//   renderArea->setDynamicStateOff();
//}

MainViewer::MainViewer()
{
   renderArea = new RenderArea(this);
   renderArea->setOffset(40,40);
   renderArea->setCellSize(3);
   renderArea->Hexitec_logoOn();
   renderArea->toolDisplayOn();
   renderArea->colorBarOn();
   renderArea->setDynamicStateOn();

   this->setCentralWidget(renderArea);
   this->setAutoFillBackground(true);

   setAcceptDrops(true);
}

// This draws the images using a single channel of data. It is called by a signal from the plotter.
void MainViewer::renderChannel(double value)
{
   renderArea->renderChannel(value, DataModel::instance()->getActiveSlice());
   renderArea->title(DataModel::instance()->getActiveSlice()->objectName() + " channel " + QString::number(DataModel::instance()->getActiveSlice()->valueToChannel(value)));
   renderArea->update();
}

// This draws images summing within +/- nvalue channels of the channel corresponding to value
void MainViewer::renderSum(double value, int nvalue)
{
   int n = DataModel::instance()->getActiveSlice()->valueToChannel(value);
   int minX = n - nvalue;
   int maxX = n + nvalue;

   renderArea->renderSum(minX, maxX, DataModel::instance()->getActiveSlice());
   renderArea->title(DataModel::instance()->getActiveSlice()->getTitle());
   renderArea->update();
   //applicationOutput->writeMessage("Sum over channel range: " + QString::number(minX) + "..." + QString::number(maxX));
}
// This draws all images summing from channel corresponding to
void MainViewer::renderSum(double minX, double maxX)
{
   int min = DataModel::instance()->getActiveSlice()->valueToChannel(minX);
   int max = DataModel::instance()->getActiveSlice()->valueToChannel(maxX);

   //applicationOutput->writeMessage("Sum over channel range: " + QString::number(min) + "..." + QString::number(max));
   //applicationOutput->writeMessage("Sum over X range: " + QString::number(minX) + "..." + QString::number(maxX));

   // main display
   renderArea->renderSum(min, max, DataModel::instance()->getActiveSlice());
   renderArea->title(DataModel::instance()->getActiveSlice()->getTitle());
   renderArea->update();
}

/* ThumbViewer sends this as a signal when the active slice is changed. The RenderArea sent is one of the
  ThumbNails. */
void MainViewer::activeSliceChanged(RenderArea *selected)
{
   if (selected == NULL)
   {
      renderArea->imageData.resize(0,0);
      renderArea->title("");
   }
   else
   {
      renderArea->setImageData(selected->imageData);
      renderArea->title(selected->title());
   }
   renderArea->setDynamicStateOn();
   renderArea->update();
}

bool MainViewer::isHeld()
{
   return renderArea->isHeld();
}
void MainViewer::dragEnterEvent(QDragEnterEvent *event)
{
   //setText(tr("<drop content>"));
   //setBackgroundRole(QPalette::Highlight);

   event->acceptProposedAction();
   // emit changed(event->mimeData());
}

void MainViewer::dragMoveEvent(QDragMoveEvent *event)
{
   QStringList fileNames = interpretMimeData(event->mimeData());

   if (Slice::fileNameListValid(fileNames))
   {
      event->acceptProposedAction();
   }
   else
   {
      event->setDropAction(Qt::IgnoreAction);
   }
}

void MainViewer::dropEvent(QDropEvent *event)
{
   QStringList fileNames = interpretMimeData(event->mimeData());
   if(fileNames.size() > 0)
   {
      emit readFiles(fileNames);
   }
   // setBackgroundRole(QPalette::Dark);
   event->acceptProposedAction();
}

void MainViewer::dragLeaveEvent(QDragLeaveEvent *event)
{
   //clear();
   event->accept();
}

QStringList MainViewer::interpretMimeData(const QMimeData *mimeData)
{
   QStringList fileNames;
   if (mimeData->hasUrls())
   {
      QList<QUrl> urlList = mimeData->urls();
      for (int i = 0; i < urlList.size(); ++i)
      {
         QString fileName = urlList.at(i).path();
         if (fileName.startsWith("/"))
         {
            fileName.remove(0, 1);
         }
         fileNames.append(fileName);
      }
   }
   return fileNames;
}



