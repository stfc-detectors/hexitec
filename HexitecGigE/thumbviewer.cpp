#include "thumbviewer.h"
#include <QMouseEvent>
#include "mainwindow.h"

ThumbViewer::ThumbViewer(QWidget *parent) :
   QWidget(parent)
{
   setBackgroundRole(QPalette::Base);
   setAutoFillBackground(true);
   cellSizeX = 1;
   cellSizeY = 1;
   offsetX = 30;
   offsetY = 30;
   sameCellSizes = true;
   thumbOffsetX = 0;
   thumbOffsetY = 50;
   showColorBars = false;
   setMouseTracking(true);
   thumbUnderMouse = -1;

   thumbWindow = new QMainWindow();
   thumbWindow->setCentralWidget(this);
   thumbWindow->addToolBar(createToolBar());
}

QToolBar *ThumbViewer::createToolBar()
{
   QToolBar *thumbToolBar = new QToolBar ;

   QAction *thumbZoomInAct = new QAction(QIcon(":/images/zoomIn.png"), tr(""),this);
   QAction *thumbZoomOutAct = new QAction(QIcon(":/images/zoomOut.png"), tr(""),this);
   QAction *colorBarsToggle = new QAction(QIcon(":/images/lockSTAR.png"), tr(""),this);
   QAction *cycleThumbColors = new QAction(QIcon(":/images/colorWheel.png"), tr(""),this);

   thumbZoomInAct->setText(tr("Zoom In"));
   thumbZoomOutAct->setText(tr("Zoom Out"));
   colorBarsToggle->setText(tr("Toggle Colour Bars"));
   cycleThumbColors->setText(tr("Cycle Colour Map"));

   thumbToolBar->addAction(thumbZoomInAct);
   thumbToolBar->addAction(thumbZoomOutAct);
   thumbToolBar->addAction(colorBarsToggle);
   thumbToolBar->addAction(cycleThumbColors);

   connect(thumbZoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));
   connect(thumbZoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));
   connect(colorBarsToggle, SIGNAL(triggered()), this, SLOT(colorBarsToggle()));
   connect(cycleThumbColors, SIGNAL(triggered()), this, SLOT(cycleColorMap()));

   return thumbToolBar;
}

void ThumbViewer::paintEvent(QPaintEvent * /* event */)
{
   QPainter painter(this);

   if (thumbNails.size() == 0)
   {
      return;
   }

   thumbRect.resize(0);
   QPoint pos;
   pos.setX(offsetX);
   pos.setY(offsetY);

   int maxHeight = 0;
   for (int i = 0 ; i < thumbNails.size(); ++i)
   {
      if (sameCellSizes)
         thumbNails[i]->setCellSize(cellSizeX,cellSizeY);

      if (thumbNails[i]->height() > maxHeight)
         maxHeight = thumbNails[i]->height();
      // This bit moves y onto another line if this one won't fit
      if (i > 0 && pos.x() + thumbNails[i]->imageData.width()*cellSizeX > width())
      {
         // move to next line before draw
         pos.setX(offsetX);
         pos.setY(pos.y() + maxHeight + thumbOffsetY - 20);  // -20 deal with title
         thumbNails[i]->setOffset(pos.x(),pos.y());
         thumbNails[i]->paintWidget(painter);
      }
      thumbNails[i]->setOffset(pos.x(),pos.y());
      thumbNails[i]->paintWidget(painter);
      // RenderArea should store its own QRect but this is a refactor too far at the moment.
      thumbRect.push_back(QRect(pos, QSize(thumbNails[i]->imageData.width()*cellSizeX, thumbNails[i]->height())));
      pos.setX(pos.x() + thumbNails[i]->width() + thumbOffsetX);
      pos.setY(pos.y());
   }

   QPen pen(Qt::blue, 2, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);
   painter.setPen(pen);  
   if (thumbUnderMouse != -1)
   {
      painter.drawRect(thumbRect[thumbUnderMouse]);
   }
   pen.setColor(QColor(Qt::red));
   painter.setPen(pen);
   painter.drawRect(thumbRect[activeSlice]);
}

void ThumbViewer::mouseMoveEvent(QMouseEvent *event)
{
   if (thumbNails.size() == 0) return ;

   QPoint widgetPosition = event->pos();

   thumbUnderMouse = -1;

   for (int i = 0 ; i < thumbRect.size() ; ++i)
   {
      if(thumbRect[i].contains(widgetPosition))
      {
         thumbUnderMouse = i;
        break;
      }
   }

   update();
}

void ThumbViewer::wheelEvent(QWheelEvent *event)
{
   if (thumbNails.size() == 0) return ;

   int numDegrees = event->delta();

   if (numDegrees > 0)
      zoomIn();
   else
      zoomOut();
   update();
}

/* This is essential. It is possible for mouseMoveEvent to leave thumbOverMouse on (probably because of
   compression of move events) which causes disaster if you then delete the active slice.
   */
void ThumbViewer::leaveEvent(QEvent *event)
{
   thumbUnderMouse = -1;
}

void  ThumbViewer::mousePressEvent(QMouseEvent *)
{
}

void  ThumbViewer::mouseDoubleClickEvent(QMouseEvent *event)
{
   if (thumbNails.size() == 0) return ;

   QPoint widgetPosition = event->pos();

   thumbUnderMouse = -1;

   for (int i = 0 ; i < thumbRect.size() ; ++i)
   {
      if(thumbRect[i].contains(widgetPosition))
      {
         thumbUnderMouse = i;
         break;
      }
   }

   if (thumbUnderMouse != -1)
   {
      activeSlice = thumbUnderMouse;
      DataModel::instance()->setActiveSlice(DataModel::instance()->sliceAt(thumbUnderMouse));
      activeSliceChanged(thumbNails[activeSlice]);
      thumbUnderMouse = -1;
      update();
   }
}

void  ThumbViewer::mouseReleaseEvent(QMouseEvent *)
{
}

void ThumbViewer::setCellSize(int s)
{
   cellSizeX = s;
   cellSizeY = s;
}

void ThumbViewer::setCellSize(int sizeX, int sizeY)
{
   cellSizeX = sizeX;
   cellSizeY = sizeY;
}

void ThumbViewer::setOffset(int X, int Y)
{
   offsetX = X;
   offsetY = Y;
}

void ThumbViewer::colorBarsToggle()
{
   showColorBars = ! showColorBars;
   if (showColorBars)
   {
      for (int i = 0; i < thumbNails.size() ; ++i)
      {
         thumbNails[i]->colorBarOn();
         thumbNails[i]->axes.autoScaleZ = true;
      }
   }
   else
   {
      for (int i = 0; i < thumbNails.size() ; ++i)
         thumbNails[i]->colorBarOff();
   }
   update();
}

void ThumbViewer::addSlice(Slice *slice)
{
   RenderArea *newThumbNail = new RenderArea();
   newThumbNail->renderSum(slice);
   newThumbNail->title(slice->objectName());
   newThumbNail->setColorBar(showColorBars);
   thumbNails.push_back(newThumbNail);
   // ThumbViewer still has an activeSlice integer which is set to the last slice added (at first)
   activeSlice = thumbNails.size() - 1;
   update();
}

void ThumbViewer::deleteSlice(Slice *slice)
{
   int i;
   int toRemove = -1;
   for (i = 0; i < thumbNails.size(); i++)
   {
      if (thumbNails.at(i)->title() == slice->objectName())
      {
         toRemove = i;
         break;
      }
   }
   if (toRemove != -1)
   {
      thumbNails.remove(toRemove);
      DataModel::instance()->deleteSlice(slice);
      // If the slice removed was the activeSlice then we need to try to replace it
      if (toRemove == activeSlice)
      {
         if (activeSlice == 0 && thumbNails.size() > 0 )
         {
              //deliberately do nothing, what was element 1 will now be element 0 and should
              //become the new active slice
         }
         else
         {
             activeSlice--;
         }
         if (activeSlice >= 0)
         {
            DataModel::instance()->setActiveSlice(DataModel::instance()->sliceAt(activeSlice));
            activeSliceChanged(thumbNails[activeSlice]);
         }
         else
         {
            DataModel::instance()->setActiveSlice(NULL);
            activeSliceChanged(NULL);
         }
      }
      // If the Slice removed wasn't the activeSlice but was before it in the list then we need to change
      // activeSlice. Here we are only changing its value - the actual activeSlice hasn't changed just its
      // position in the thumbNails.
      else if (toRemove < activeSlice)
      {
         activeSlice--;
      }
   }
   update();
}

void ThumbViewer::zoomIn()
{
   cellSizeX++;
   cellSizeY++;
   update();
}

void ThumbViewer::zoomOut()
{

   if (cellSizeX > 1 && cellSizeY > 1)
   {
      cellSizeX--;
      cellSizeY--;
      update();
   }
}

void ThumbViewer::cycleColorMap()
{
   for (int i = 0; i < thumbNails.size() ; ++i)
      thumbNails[i]->colorMap.nextColorMap();
   update();

//   writeMessage("This is a test message from RenderArea::testDevelopment() :" + QString::number(123456789));
}

void ThumbViewer::scaleZAxes(AxisSettings axes)
{  
   // There is an asymmetrical connection between autoScaleZ and showColorBar - see the autoScaleZ and
   // colorBarsToggle.
   if (!autoScaleZ())
   {
      for (int k = 0 ; k < thumbNails.size() ; ++k)
      {
         thumbNails[k]->axes.autoScaleZ = false;
         thumbNails[k]->axes.minZ = axes.minZ;
         thumbNails[k]->axes.maxZ = axes.maxZ;
      }
   }
}

QMainWindow *ThumbViewer::getMainWindow()
{
   return thumbWindow;
}

void ThumbViewer::renderChannel(double value)
{
   for (int k = 0 ; k < thumbNails.size() ; ++k)
   {
      thumbNails[k]->renderChannel(value, DataModel::instance()->sliceAt(k));
   }

   scaleZAxes(MainViewer::instance()->getAxes());
   update();
}

void ThumbViewer::renderSum(double minX, double maxX)
{
   int min = DataModel::instance()->getActiveSlice()->valueToChannel(minX);
   int max = DataModel::instance()->getActiveSlice()->valueToChannel(maxX);

   for (int k = 0 ; k < thumbNails.size() ; ++k)
   {
      thumbNails[k]->renderSum(min, max, DataModel::instance()->sliceAt(k));
   }

   scaleZAxes(MainViewer::instance()->getAxes());
   update();
}

void ThumbViewer::renderSum(double xVal, int range)
{
   int n = DataModel::instance()->getActiveSlice()->valueToChannel(xVal);
   int min = n - range;
   int max = n + range;

   for (int k = 0 ; k < thumbNails.size() ; ++k)
   {
      thumbNails[k]->renderSum(min, max, DataModel::instance()->sliceAt(k));
   }

   scaleZAxes(MainViewer::instance()->getAxes());
   update();
}

void ThumbViewer::testDevelopment()
{
//   writeMessage("This is a test message from RenderArea::testDevelopment() :" + QString::number(123456789));
}

