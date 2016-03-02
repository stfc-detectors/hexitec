/*
RenderArea class for TwoEasy code
- this is the surface mapping widget
S D M Jacques 02 May 2011

NOTE:
This includes a TEMPORARY FIX for mouse events when the RenderArea does not
display a slice because activeSlice = -1. A bool will be implemented to say
whether the RenderArea is active and if not it will switch off mouse tracking.
*/

#include <QtGui>
#include "renderarea.h"
#include <qcolor.h>
#include <QMouseEvent>
#include "mainwindow.h"
#include <QString>
#include <QDebug>

RenderArea::RenderArea(QWidget *parent) :
   QWidget(parent)
{
   setDynamicStateOff();
   setMouseEnabled(false);
   toolDisplayOff();
   showHexitec_Logo = false;
   cellSizeX = 1;
   cellSizeY = 1;
   offsetX = 0;
   offsetY = 0;
   widthValue = 0;
   heightValue = 0;
   axes.minZ = 0.0;
   axes.maxZ = 255.0;
   maxColor = 255.0;
   zMode = 0;
   showColorBars = true;

   overPixelState = false;
   overColorBarState = false;
   currentPixel.setX(-1);
   currentPixel.setY(-1);
   selectedPixels.resize(0);

   setBackgroundRole(QPalette::Base);
   setAutoFillBackground(true);
   setAcceptDrops(true);

   // others
   axesOff();
   zoomOff();
   holdOff();
   gridOn();
   titleOn();
   colorBarOff();
   showRubberBand = false;
   // labels
   title("");
   xLabel("x");
   yLabel("y");
   // mouse
   mouseButtonPressed = false;
   //    mouseDoubleClicked = false;
   this->setMouseTracking(true);

   colorMapEditor = new ColorMapEditor(this);
   connect(colorMapEditor, SIGNAL(setZMode(int)), this, SLOT(setZMode(int)));
   connect(colorMapEditor, SIGNAL(setNSigma(double)), this, SLOT(setNSigma(double)));
   connect(colorMapEditor, SIGNAL(setNMean(double)), this, SLOT(setNMean(double)));
   connect(colorMapEditor, SIGNAL(setMinimumColour(int)), this, SLOT(setMinimumColour(int)));
   connect(colorMapEditor, SIGNAL(setMaximumColour(int)), this, SLOT(setMaximumColour(int)));
   nSigma = 1.5;
   nMean = 2.0;

   // display
   update();
   setMouseEnabled(true);
}

void RenderArea::setMouseEnabled(bool enabled)
{
   mouseEnabled = enabled;
}


void RenderArea::paintEvent(QPaintEvent * /* event */)
{
   QPainter painter(this);
   paintWidget(painter);
}

void RenderArea::paintWidget(QPainter &painter)
{
   QImage splash(":/images/Hexitec_Logo10.png");
   if (imageData.width() == 0)
   {
      painter.drawImage(offsetX, offsetY, splash);
      return;
   }

   painter.setFont(QFont("Arial", cellSizeY+5));

   int cellPosX;
   int cellPosY;

   if (imageData.width() < 10 || imageData.height() < 10)
   {
      titleOff();
   }
   if (imageData.height() < 10)
   {
      offsetY = 30;
   }


   if (showHexitec_Logo)
   {
      QImage Hexitec_logo(":/images/Hexitec_logo.png");
      painter.setOpacity(0.8);
      painter.drawImage(75 + offsetX + imageData.width() * cellSizeX, imageData.height() * cellSizeY - 28, Hexitec_logo);
   }

   // define the axis boundaries
   axesBox.setLeft(offsetX);
   axesBox.setRight(offsetX + (imageData.width()) * cellSizeX - 1);
   axesBox.setBottom(offsetY + (imageData.height()) * cellSizeY - 1);
   axesBox.setTop(offsetY);

   widthValue = axesBox.width();
   heightValue = axesBox.height();

   // tool status
   if (showTools)
   {
      painter.setOpacity(1.0);
      //
      if (!axes.autoScaleZ)
      {
         QImage lockCSmall(":/images/lockCSmall.png");
         painter.drawImage(axesBox.left() - 30, axesBox.top(), lockCSmall);
      }

      if (holdState)
      {
         QImage holdSmall(":/images/holdSmall.png");
         painter.drawImage(axesBox.left() - 30, axesBox.top() + 27, holdSmall);
      }
   }

   // set the title
   if (showTitle)
   {
      QRect titleBar (axesBox.left()-10, axesBox.top()-20, axesBox.width()+10, axesBox.top());
      painter.drawText(titleBar, Qt::AlignHCenter | Qt::TextWordWrap , title());
   }

   axes.adjust();
   if (cellSizeX < 2)
      axes.numZTicks = 2;

   painter.setOpacity(colorMap.opacity);
   QColor cellColor;
   double span = axes.spanZ();
   for (int x = 0; x < imageData.width() ; x++) {
      for (int y = 0; y < imageData.height() ; y++) {
         cellPosX = x * cellSizeX + offsetX;
         cellPosY = y * cellSizeY + offsetY;
         QRect rect(cellPosX, cellPosY, cellSizeX, cellSizeY);
         cellColor = colorMap.color(imageData[x][y]/span);
         painter.setBrush(QColor(cellColor));
         painter.setPen(QColor(cellColor));
         if (!showGridLines)
            painter.setPen(Qt::NoPen);
         painter.drawRect(rect);
      }
   }
   painter.setBrush(Qt::NoBrush);
   painter.setPen(QColor(0,0,0));
   painter.drawRect(axesBox);
   painter.setOpacity(1.0);


   if (colorBar)
   {
      int colorBarWidth = cellSizeX * 4 + 4;
      int colorBarHeight = cellSizeY * 40;
      int colorBarPosX = axesBox.right() + cellSizeX * 4 + 4;
      int colorBarPosY = axesBox.top() +  (int) (axesBox.height()/2.0 - colorBarHeight/2) ;
      colorBarBox.setRect(colorBarPosX, colorBarPosY, colorBarWidth, colorBarHeight);
      for (int j = 0; j <= axes.numZTicks; ++j)
      {
         int y = colorBarBox.bottom() - (j * (colorBarBox.height() - 1) / axes.numZTicks);
         double label = axes.minZ + (j * axes.spanZ() / axes.numZTicks);
         //emit writeMessage(QString::number(axes.spanZ()));
         painter.setPen(QColor(0,0,0));
         painter.drawLine(colorBarBox.right() + cellSizeX, y, colorBarBox.right(), y);
         painter.drawText(colorBarBox.right() + cellSizeX*2 + 2, y - 10, 65, 20, Qt::AlignLeft | Qt::AlignVCenter, QString::number(label));
      }
      painter.setOpacity(colorMap.opacity);
      QLinearGradient gradient = colorMap.gradient;
      gradient.setStart(colorBarBox.bottomLeft());
      gradient.setFinalStop(colorBarBox.topLeft());
      painter.setBrush(gradient);
      painter.drawRect(colorBarBox.adjusted(0, 0, -1, -1));
      painter.setOpacity(1.0);
      widthValue += 2 * colorBarWidth + 65;
   }

   if (overColorBarState)
   {
      painter.setBrush(Qt::NoBrush);
      painter.setPen(QColor(Qt::red));
      painter.drawRect(colorBarBox.adjusted(0, 0, -1, -1));
   }

   if (overPixelState)
   {
      cellPosX = currentPixel.x() * cellSizeX + offsetX;
      cellPosY = currentPixel.y() * cellSizeY + offsetY;
      QRect rect(cellPosX, cellPosY, cellSizeX, cellSizeY);
      painter.setBrush(QColor(Qt::blue));
      painter.setPen(QColor(Qt::blue));
      painter.setOpacity(0.5);
      painter.drawRect(rect);
   }

   for (int i = 0; i < selectedPixels.size() ; ++i)
   {
      cellPosX = selectedPixels[i].x() * cellSizeX + offsetX;
      cellPosY = selectedPixels[i].y() * cellSizeY + offsetY;
      QRect rectSel(cellPosX , cellPosY, cellSizeX, cellSizeY);
      painter.setBrush(QColor(Qt::red));
      painter.setPen(QColor(Qt::red));
      painter.setOpacity(0.5);
      painter.drawRect(rectSel);
   }
}

void RenderArea::setCellSize(int s)
{
   cellSizeX = s;
   cellSizeY = s;
}

void RenderArea::setCellSize(int sizeX, int sizeY)
{
   cellSizeX = sizeX;
   cellSizeY = sizeY;
}

void RenderArea::setOffset(int X, int Y)
{
   offsetX = X;
   offsetY = Y;
}

void RenderArea::mouseMoveEvent(QMouseEvent *event)
{
   if (!dynamicState)
      return;

   if (!mouseEnabled)
      return;

   if (imageData.width() == 0) return;  // TEMPORARY FIX

   // need a if visible statement here
   if(mouseButtonPressed)
      return;

   QPoint widgetPosition = event->pos();
   QPoint pixel;
   pixel.setX( (int)((widgetPosition.x() - offsetX)/cellSizeX) ) ;
   pixel.setY( (int)((widgetPosition.y() - offsetY)/cellSizeY) );

   if (overPixel(pixel))
   {
      currentPixel = pixel;
      emit updatePlotter(currentPixel, false);
   }
   else
   {
      if (selectedPixels.isEmpty()) // give empty plot
         updatePlotter();
      if (!selectedPixels.isEmpty() && holdState) {}
      emit updatePlotter(pixel, false);
   }

   if (insideRectArea(colorBarBox, event->pos()))
   {
      overColorBarState = true;
   }
   else
      overColorBarState = false;

   update();
}

void RenderArea::wheelEvent(QWheelEvent *event)
{
   if (!dynamicState)
      return;

   if (!mouseEnabled)
      return;

   if (imageData.width() == 0)
      return;  // TEMPORARY FIX

   int numDegrees = event->delta();

   if (numDegrees > 0)
      zoomIn();
   else
      zoomOut();
   update();
}


void  RenderArea::mousePressEvent(QMouseEvent *)
{
   if (!dynamicState)
      return;

   if (!mouseEnabled)
      return;

   if (imageData.width() == 0)
      return;  // TEMPORARY FIX
}

void  RenderArea::mouseDoubleClickEvent(QMouseEvent *event)
{
   if (!dynamicState)
      return;

   if (!mouseEnabled)
      return;

   if (insideRectArea(colorBarBox, event->pos()) & colorMapEditor->isHidden())
      colorMapEditor->show();

   if (imageData.width() == 0) return;  // TEMPORARY FIX

   QPoint widgetPosition = (event->pos());
   QPoint pixel;
   pixel.setX( (int)((widgetPosition.x() - offsetX)/cellSizeX) ) ;
   pixel.setY( (int)((widgetPosition.y() - offsetY)/cellSizeY) );

   if (overPixel(pixel))
   {
      if (!holdState)
         selectedPixels.resize(0);

      selectedPixels.push_back(pixel);
      emit updatePlotter(currentPixel, true);
   }
   update();
}

bool RenderArea::overPixel(QPoint pixel)
{
   overPixelState  = (pixel.x() > -1 &&
                      pixel.x() < imageData.width() &&
                      pixel.y() > -1 &&
                      pixel.y() < imageData.height());
   return overPixelState;
}

bool RenderArea::insideRectArea(QRect rect, QPointF position)
{
   if (position.x() > rect.left() && position.x() < rect.right() && position.y() > rect.top() && position.y() < rect.bottom())
      return(true);
   else
      return(false);
}

void RenderArea::mouseReleaseEvent(QMouseEvent *)
{
   if (imageData.width() == 0) return;  // TEMPORARY FIX
}


void RenderArea::zoomIn()
{
   cellSizeX++;
   cellSizeY++;
   update();
}

void RenderArea::zoomOut()
{

   if (cellSizeX > 1 && cellSizeY > 1)
   {
      cellSizeX--;
      cellSizeY--;
      update();
   }
}

void RenderArea::lockCAxis()
{
   axes.autoScaleZ = !axes.autoScaleZ;
   update();
}

void RenderArea::cycleColorMap()
{
   colorMap.nextColorMap();
   update();
}

void RenderArea::dragEnterEvent(QDragEnterEvent *event)
{
   if (event->mimeData()->hasFormat("text/plain"))
      event->acceptProposedAction();
}

void RenderArea::dropEvent(QDropEvent *de)
{
   // Unpack dropped data and handle it the way you want
   QString apples;
   apples.sprintf("Contents: %s",de->mimeData()->text().toLatin1().data());
   writeMessage( apples );
}

void RenderArea::setMinZ(double value)
{
   axes.minZ = value;
   update();
}

void RenderArea::setMaxZ(double value)
{
   axes.maxZ = value;
   update();
}

void RenderArea::setLimZ(double value1, double value2)
{
   axes.minZ = value1;
   axes.maxZ = value2;
   update();
}

void RenderArea::stats()
{
   if (imageData.height() == 0)
      return;
   sumData = 0;
   minData = imageData[0][0];
   maxData = imageData[0][0];
   meanData = 0;
   for (int x = 0; x < imageData.width() ; ++x )
   {
      for (int y = 0; y < imageData.height() ; ++y )
      {
         if (imageData[x][y] > maxData)
            maxData = imageData[x][y];
         if (imageData[x][y] < minData)
            minData = imageData[x][y];
         sumData += imageData[x][y];
      }
   }
   meanData = sumData / (1.0 * imageData.width() * imageData.height());

   // Now work out the variance
   double diff;
   sigma2 = 0;
   for (int x = 0; x < imageData.width() ; ++x )
   {
      for (int y = 0; y < imageData.height() ; ++y )
      {
         diff = (meanData - imageData[x][y]);
         sigma2 += diff*diff;
      }
   }
   sigma2 = sigma2 / (1.0 * imageData.width() * imageData.height());
   sigma = sqrt(sigma2);
}

void RenderArea::setImageData(SArray<double> imageData)
{
   this->imageData = imageData;
   if (axes.autoScaleZ)
   {
      stats();
      autoScaleZ();
   }
}

AxisSettings RenderArea::getAxes()
{
   return axes;
}

void RenderArea::renderSum(Slice *slice)
{
   this->imageData.resize(slice->getGridSizeX(), slice->getGridSizeY());
   this->imageData = slice->sumImage();
   if (this->axes.autoScaleZ)
   {
      this->stats();
      this->autoScaleZ();
   }
   if (slice->getSummedImageY() != NULL)
   {
      emit updatePlotter(slice->getXData(0, 0), slice->getSummedImageY(), slice->getNumberOfBins());
   }

}

void RenderArea::renderSum(int min, int max, Slice *slice)
{
   this->imageData.resize(slice->getGridSizeX(), slice->getGridSizeY());
   this->imageData = slice->sumImage(min, max);
   if (this->axes.autoScaleZ)
   {
      this->stats();
      this->autoScaleZ();
   }
}

void RenderArea::renderChannel(double value, Slice *slice)
{
   this->imageData.resize(slice->getGridSizeX(), slice->getGridSizeY());
   this->imageData = slice->channelImage(value);
   if (this->axes.autoScaleZ)
   {
      this->stats();
      this->autoScaleZ();
   }
}

void RenderArea::colorBarsToggle()
{
   showColorBars = ! showColorBars;
   if (showColorBars)
   {
      this->colorBarOn();
      this->axes.autoScaleZ = true;
   }
   else
   {
      this->colorBarOff();
   }
   update();
}

void RenderArea::setColorBar(bool colorBar)
{
   this->colorBar = colorBar;
}

void RenderArea::setZMode(int value)
{
   zMode = value;
   autoScaleZ();
}

void RenderArea::setNSigma(double value)
{
   nSigma = value;
   autoScaleZ();
}

void RenderArea::setNMean(double value)
{
   nMean = value;
   autoScaleZ();
}

void RenderArea::setMinimumColour(int minimum)
{
   if (minimum < axes.maxZ)
   {
      axes.minZ = minimum;
//	  qDebug() << "RenderArea:: update commented out!";
//      update();
   }
}

void RenderArea::setMaximumColour(int maximum)
{
   if (maximum > axes.minZ)
   {
       axes.maxZ = maximum;
       update();
   }
}

void RenderArea::autoScaleZ()
{
   if (zMode == 0)
   {
      axes.minZ = 0.0;
      axes.maxZ = meanData * nMean;

   }
   if (zMode == 1)
   {
      axes.minZ = meanData - nSigma * sigma;
      axes.maxZ = meanData + nSigma * sigma;
   }
   if (zMode == 2)
   {
      axes.minZ = minData;
      axes.maxZ = maxData;
   }

   // The drawing goes wrong if the axes limits are the same (e.g. if all the data values are zero).
   if (axes.maxZ == axes.minZ)
   {
       axes.maxZ += 1.0;
   }

   update();
}

void RenderArea::testDevelopment()
{
//   writeMessage("This is a test message from RenderArea::testDevelopment() :" + QString::number(123456789));
}

