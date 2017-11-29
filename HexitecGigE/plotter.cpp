/*
NOTE:
This includes a TEMPORARY FIX for mouse events when the Plotter does not
contain a plot because activeSlice = -1. A bool will be implemented to say
whether the RenderArea is active and if not it will switch off mouse tracking.

 S D M Jacques 02 May 2011
*/
#include "plotter.h"
#include <QtGui>
#include <QColor>
#include <QMouseEvent>
#include "mainwindow.h"
#include <math.h>
#include <QPoint>
#include "parameters.h"
#include <QSettings>

Plotter::Plotter(QWidget *parent) :
   QWidget(parent)
{
   QStringList colorList;
   colorList << "#DC143C" // Crimson
             << "#3D9140" // Cobalt Green
             << "#4169E1" // Royal Blue*/
             << "#FF8000" // Orange
             << "#D02090" // Violet Red
             << "#40E0D0" // Turquoise
             << "#E3CF57" // Banana
             << "#708090" // Slate Grey
             << "#9B30FF" // Purple 1
             << "#8E388E" // SGI Beet
             << "#808000" // Olive
             << "#3D9140"; // Cobalt Green
   colorIndex = 0;

   toolTip = new QString("");
   this->setToolTip(*toolTip);

   QSettings settings(QSettings::UserScope, "TEDDI", "HexitecGigE");
   QString twoEasyFilename = Parameters::twoEasyIniFilename;
   int maxBins;

   if (settings.contains("hexitecGigEIniFilename"))
   {
      twoEasyFilename = settings.value("hexitecGigEIniFilename").toString();
   }

   twoEasyIniFile = new IniFile(twoEasyFilename);

   if ((maxBins = twoEasyIniFile->getInt("Processing/Maximum_Bins")) != QVariant(INVALID))
   {
      this->maxBins = maxBins;
   }

   summedCurve = new Curve();
   summedCurve->xData.resize(this->maxBins);
   summedCurve->yData.resize(this->maxBins);
   lastCurve.xData.resize(this->maxBins);
   lastCurve.yData.resize(this->maxBins);

   foreach (QString colorName, colorList)
   {
      QColor color = QColor(colorName);
      color.setNamedColor(colorName);
      colors.append(color);
   }

   setAxesAction = new QAction("Set Axes...",this);
   displayLegendsAction = new QAction("Display Legends...",this);
   displayPlotValueAction = new QAction("Display Plot Values",this);
   displayLegendsAction->setCheckable(true);
   displayPlotValueAction->setCheckable(true);
   plotterMenu.addAction(setAxesAction);
   plotterMenu.addAction(displayLegendsAction);
   plotterMenu.addAction(displayPlotValueAction);
   setAxesDialog = new SetAxesDialog(this);
   displayLegendsDialog = new DisplayLegendsDialog(this);

   connect(setAxesDialog, SIGNAL(accepted()), this, SLOT(setAxesAccepted()));
   connect(setAxesDialog, SIGNAL(rejected()), this, SLOT(setAxesRejected()));
   connect(setAxesDialog, SIGNAL(apply()), this, SLOT(setAxesApplied()));
   connect(displayLegendsDialog, SIGNAL(finished(int)), this, SLOT(displayLegendsFinished(int)));

   setDynamicStateOff();
   setBackgroundRole(QPalette::Base);
   setAutoFillBackground(true);
   pixelAddition = false;
   currentCurve = -1;
   currentZoom = 0;
   zoomStack.resize(currentZoom + 1);
   // others
   axesOn();
   zoomOn();
   showRubberBand = false;
   holdOff();
   gridOff();
   xExploreOff();
   sumOff();
   spSumState = true;
   numberOfSumPoints = 5;
   numberLeftPoints = 2;
   numberRightPoints = 2;
   wheelValue = 0;
   // labels
   title("");
   xLabel("Bins");
   yLabel("Pixel Y");
   summedYLabel("Sum Y");
   // margins
   setMargins(55, 80, 35, 45);
   // mouse
   mouseButtonPressed = false;
   mouseDoubleClicked = false;
   this->setMouseTracking(true);

   channelExplorerSpinBox = new QSpinBox(this);
   channelExplorerSpinBox->hide();
//   backgroundRemoveSpinBox = new QSpinBox(this);
//   backgroundRemoveSpinBox->hide();
   connect(channelExplorerSpinBox, SIGNAL(valueChanged(int)), this, SLOT(CESpinBoxCallBack()));

   plotWindow = new QMainWindow();
   plotWindow->setCentralWidget(this);
   plotWindow->addToolBar(createToolBar()) ;

   plotWindow->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(plotWindow, SIGNAL(customContextMenuRequested(const QPoint&)),
           this, SLOT(showContextMenu(const QPoint&)));

   setDynamicStateOn();
   // display
   update();
}

QToolBar *Plotter::createToolBar()
{
   QToolBar *plotToolBar = new QToolBar ;

   QAction *zoomInAct = new QAction(QIcon(":/images/zoomIn.png"), tr(""),this);
   QAction *zoomOutAct = new QAction(QIcon(":/images/zoomOut.png"), tr(""),this);
//   QAction *yLockToggleAct = new QAction(QIcon(":/images/unlockY.png"), tr(""),this);
   QAction *gridToggle = new QAction(QIcon(":/images/grid.png"), tr(""),this);
   QAction *holdPlot = new QAction(QIcon(":/images/hold.png"), tr(""),this);
   QAction *clearPlot = new QAction(QIcon(":/images/clean.png"), tr(""),this);
   QAction *exploreToggle = new QAction(QIcon(":/images/explore.png"), tr(""),this);
   QAction *sumToggleAct = new QAction(QIcon(":/images/Sum.png"), tr(""),this);

   zoomInAct->setText("Zoom In");
   zoomOutAct->setText(tr("Zoom Out"));
//   yLockToggleAct->setText(tr("toggle Y Axis lock"));
   gridToggle->setText(tr("Toggle Grid"));
   holdPlot->setText(tr("Toggle Hold"));
   clearPlot->setText(tr("Clear Plot"));
   exploreToggle->setText(tr("Toggle X Explorer"));
   sumToggleAct->setText(tr("Toggle Sum Explorer"));

   plotToolBar->addAction(zoomInAct);
   plotToolBar->addAction(zoomOutAct);
//   plotToolBar->addAction(yLockToggleAct);
   plotToolBar->addAction(gridToggle);
   plotToolBar->addAction(holdPlot);
   plotToolBar->addAction(clearPlot);
   plotToolBar->addAction(exploreToggle);
   plotToolBar->addAction(sumToggleAct);

   connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));
   connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));
//   connect(yLockToggleAct, SIGNAL(triggered()), this, SLOT(yLockToggle()));
   connect(gridToggle, SIGNAL(triggered()), this, SLOT(gridToggle()));
   connect(holdPlot, SIGNAL(triggered()), this, SLOT(holdPlot()));
   connect(clearPlot, SIGNAL(triggered()), this, SLOT(clearPlot()));
   connect(exploreToggle, SIGNAL(triggered()), this, SLOT(exploreToggle()));
   connect(sumToggleAct, SIGNAL(triggered()), this, SLOT(sumToggle()));

   return plotToolBar;
}

void Plotter::paintEvent(QPaintEvent * /* event */)
{
   QPainter painter(this);
   QImage image(":/images/Hexitec_logo10.png");
   double spanX;
   int numXTicks;

   // define the axis boundaries
   axesBox.setLeft(leftMargin);
   axesBox.setRight(width() - rightMargin);
   axesBox.setBottom(height() - bottomMargin);
   axesBox.setTop(topMargin);

   // pen attributes
   painter.setPen(Qt::NoBrush);
   painter.setFont(QFont("Arial", 9));
   painter.setPen(QColor(0,0,0));

   // draw the axis
   if (showAxes)
   {
      if (currentZoom == 0)
      {
         spanX = zoomStack[currentZoom].maxX - zoomStack[currentZoom].minX;
         numXTicks = 4;
      }
      else
      {
         spanX = zoomStack[currentZoom].spanX();
         numXTicks = zoomStack[currentZoom].numXTicks;
      }

      for (int i = 0; i <= numXTicks; ++i)
      {
         int x = axesBox.left() + (i * (axesBox.width() - 1) / numXTicks);
         double label = zoomStack[currentZoom].minX + (i * spanX / numXTicks);

         if (showGridLines)
         {
            painter.setPen(QColor(Qt::lightGray));
            painter.drawLine(x, axesBox.top(), x, axesBox.bottom());
         }

         painter.setPen(QColor(0,0,0));
         painter.drawLine(x, axesBox.bottom(), x, axesBox.bottom() + 5);
         painter.setFont(QFont("Arial", 8));

         painter.drawText(x - 50, axesBox.bottom() + 8, 100, 15, Qt::AlignHCenter | Qt::AlignTop, QString::number(label, 'g', 4));
      }

      for (int j = 0; j <= zoomStack[currentZoom].numYTicks; ++j)
      {
         int y = axesBox.bottom() - (j * (axesBox.height() - 1) / zoomStack[currentZoom].numYTicks);
         double label = zoomStack[currentZoom].minY + (j * zoomStack[currentZoom].spanY() / zoomStack[currentZoom].numYTicks);

         if (showGridLines)
         {
            painter.setPen(QColor(Qt::lightGray));
            painter.drawLine(axesBox.left(), y, axesBox.right(), y);
         }

         painter.setPen(colors.at(1));
         painter.drawLine(axesBox.left() - 5, y, axesBox.left(), y);
         painter.drawText(axesBox.left() - 58, y - 10, leftMargin - 5, 20, Qt::AlignRight | Qt::AlignVCenter, QString::number(label, 'g', 4));
      }

      painter.setPen(QColor(0,0,0));
      double spanSummedY = (summedCurve->maxYData - summedCurve->minYData);

      for (int j = 0; j <= zoomStack[currentZoom].numYTicks; ++j)
      {
         int y = axesBox.bottom() - (j * (axesBox.height() - 1) / zoomStack[currentZoom].numYTicks);
         double label = zoomStack[currentZoom].minSummedY + (j * zoomStack[currentZoom].spanSummedY() / zoomStack[currentZoom].numYTicks);

         painter.drawLine(axesBox.right() + 5, y, axesBox.right(), y);
         painter.drawText(axesBox.right() + 10, y - 10, rightMargin + 5, 20, Qt::AlignLeft | Qt::AlignVCenter, QString::number(label, 'g', 4));
      }
      // add axis labels
      painter.setFont(QFont("Arial", 9));

      QRect rectXLabel(leftMargin, axesBox.bottom() + 25 , axesBox.width(), bottomMargin - 25);
      painter.drawText(rectXLabel, Qt::AlignHCenter | Qt::TextWordWrap, xLabel());

      painter.setPen(colors.at(1));
      QRect rectYLabel(leftMargin/10, topMargin - 20, leftMargin, axesBox.height());
      painter.drawText(rectYLabel, Qt::AlignTop | Qt::AlignRight, yLabel());

      painter.setPen(QColor(0,0,0));
      QRect rectSummedYLabel(leftMargin + axesBox.width(), topMargin - 20, axesBox.width(), axesBox.height());
      painter.drawText(rectSummedYLabel, Qt::AlignTop, summedYLabel());
   }
   painter.setPen(QColor(0,0,0));

   // tool status
   {
      if (!zoomStack[currentZoom].autoScaleX && !zoomStack[currentZoom].autoScaleY)
      {
         QImage lockSmall(":/images/lockSmall.png");
         painter.drawImage(axesBox.right() + 50, axesBox.top(), lockSmall);
      }
      if (zoomStack[currentZoom].autoScaleX && !zoomStack[currentZoom].autoScaleY)
      {
         QImage lockYSmall(":/images/lockYSmall.png");
         painter.drawImage(axesBox.right() + 50, axesBox.top(), lockYSmall);
      }
      if (!zoomStack[currentZoom].autoScaleX && zoomStack[currentZoom].autoScaleY)
      {
         QImage lockXSmall(":/images/lockXSmall.png");
         painter.drawImage(axesBox.right() + 50, axesBox.top(), lockXSmall);
      }
      if (holdState)
      {
         QImage holdSmall(":/images/holdSmall.png");
         painter.drawImage(axesBox.right() + 50, axesBox.top() + 27, holdSmall);
      }
      if (xExploreState)
      {
         QImage exploreSmall(":/images/exploreSmall.png");
         painter.drawImage(axesBox.right() + 50, axesBox.top() + 54, exploreSmall);
      }
      if (sumState)
      {
         QImage sumSmall(":/images/SumSmall.png");
         painter.drawImage(axesBox.right() + 50, axesBox.top() + 54, sumSmall);
      }

   }

   // draw the bounding frame
   painter.setPen(QColor(0,0,0));
   painter.drawRect(axesBox.adjusted(0, 1, -1, 0));

   // set the title
   QRect titleBar (axesBox.left()-10, axesBox.top()-30, axesBox.width()+10, axesBox.top());
   painter.setPen(colors.at(colorIndex));
   painter.drawText(titleBar, Qt::AlignHCenter | Qt::TextWordWrap , title());
   painter.setPen(QColor(0,0,0));

   painter.setRenderHint(QPainter::Antialiasing);
   painter.setClipRect(axesBox);
   // define these for speed

   QPainterPath summedPath;

   scaleX = ((double) axesBox.width()) / (zoomStack[currentZoom].spanX());
   scaleSummedY = ((double) axesBox.height()) / (zoomStack[currentZoom].spanSummedY());

   summedCurve->curvePen.setBrush(QColor(Qt::black));
   paintSummedCurve(&summedPath);
   painter.strokePath(summedPath, summedCurve->curvePen);

   scaleY = ((double) axesBox.height()) / (zoomStack[currentZoom].spanY());

   for (int j = 0 ; j < curve.size() ; ++j)
   {
      QPainterPath path;
      paintCurve(curve[j], &path);
      painter.strokePath(path, curve[j]->curvePen);
   }

   double x,y;

   if (xExploreState)
   {
      if (xExploreFreeze)
      {
         painter.setPen(QColor(Qt::red));
      }
      else
      {
         painter.setPen(QColor(Qt::blue));
      }

      x = (mouseAxesPos.x()-zoomStack[currentZoom].minX) * scaleX;

      if (channelExplorerSpinBox->value() == 0)
      {
         painter.drawLine((int) x + axesBox.left(), axesBox.bottom(), (int) x + axesBox.left(), axesBox.top());
      }
      else // need to chane this so that it displays a rectangle
      {
         painter.drawLine((int) x + axesBox.left(), axesBox.bottom(), (int) x + axesBox.left(), axesBox.top());
      }

      QRect spinBoxPos(axesBox.right() + 50, axesBox.top() + 81, 35, 20);
      channelExplorerSpinBox->setGeometry(spinBoxPos);
      channelExplorerSpinBox->show();
//      QRect spinBoxPos2(axesBox.right() + 50, axesBox.top() + 108, 35, 20);
//      backgroundRemoveSpinBox->setGeometry(spinBoxPos2);
//      backgroundRemoveSpinBox->show();
   }

   if (!xExploreState)
   {
      channelExplorerSpinBox->hide();
//      backgroundRemoveSpinBox->hide();
   }

   if (showRubberBand)
   {
      painter.setPen(QColor(Qt::gray));
      painter.drawRect(rubberBandRect);
   }

   // add the Hexitec_logo
   painter.setOpacity(0.7);
   painter.drawImage(axesBox.left() + axesBox.width() - image.width() - 5, axesBox.top() + 5, image);
   painter.setOpacity(1.0);
}

void Plotter::paintCurve(Curve *curve, QPainterPath *path)
{
   double x, y;

   y = (curve->yData[0]-zoomStack[currentZoom].minY) * scaleY;
   x = (curve->xData[0]-zoomStack[currentZoom].minX) * scaleX;
   path->moveTo((int) x + axesBox.left(), (int) y * -1.0 + height() - bottomMargin);
   for (int i = 0 ; i <  curve->xData.size(); ++i )
   {
      if (curve->xData[i] < zoomStack[currentZoom].minX)
      {
         continue;
      }
      if (curve->xData[i] > zoomStack[currentZoom].maxX)
      {
         continue;
      }

      y = (curve->yData[i]+curve->offset-zoomStack[currentZoom].minY) * scaleY;
      x = (curve->xData[i]-zoomStack[currentZoom].minX) * scaleX;
      path->lineTo((int) x + axesBox.left(), (int) y * -1.0 + height() - bottomMargin);
   }
}

void Plotter::paintSummedCurve(QPainterPath *path)
{
   double x,y;

   y = (summedCurve->yData[0]-summedCurve->minYData) * scaleSummedY;
   x = (summedCurve->xData[0]-zoomStack[currentZoom].minX) * scaleX;
   path->moveTo((int) x + axesBox.left(), (int) y * -1.0 + height() - bottomMargin);
   for (int i = 0 ; i <  summedCurve->xData.size(); ++i )
   {
      if (summedCurve->xData[i] < zoomStack[currentZoom].minX)
      {
         continue;
      }
      if (summedCurve->xData[i] > zoomStack[currentZoom].maxX)
      {
         continue;
      }

      y = (summedCurve->yData[i]-zoomStack[currentZoom].minSummedY) * scaleSummedY;
      x = (summedCurve->xData[i]-zoomStack[currentZoom].minX) * scaleX;
      path->lineTo((int) x + axesBox.left(), (int) y * -1.0 + height() - bottomMargin);
   }
}

void Plotter::mouseMoveEvent(QMouseEvent *event)
{
   if (curve.isEmpty()) return;
   if (showRubberBand)
   {
      rubberBandRect.setBottomRight(event->pos());
      update();
   }

   if (inSideAxes(event->pos()))
   {
      mousePosition = event->pos();
      toolTipPoint = this->mapToGlobal(event->pos());

      mouseAxesPos = pixelToAxes(mousePosition);

      if (xExploreState && !xExploreFreeze)
      {
         if (channelExplorerSpinBox->value() ==  0)
         {
            emit renderChannel(mouseAxesPos.x());  // should really return mouseAxesPos here and delete selChannel stuff
         }
         else
         {
            emit renderSum(mouseAxesPos.x(), channelExplorerSpinBox->value());
         }
         update();
      }

      QString coords = QString::number(mouseAxesPos.x(), 'f', 0) + ", " + QString::number(mouseAxesPos.y(), 'f', 0);

      if (displayPlotValueAction->isChecked())
      {
         toolTip = new QString(coords);
      }
      else
      {
         toolTip = new QString("");
      }

   }
   else
   {
      toolTip = new QString("");
   }

   this->setToolTip(*toolTip);
   QToolTip::showText(toolTipPoint, *toolTip);
}

void Plotter::mousePressEvent(QMouseEvent *event)
{
   if (curve.isEmpty())
   {
      return;
   }

   if (!inSideAxes(event->pos()))
   {
      return;
   }

   if (zoomState  && event->button() == Qt::LeftButton)
   {
      showRubberBand = true;
      rubberBandRect.setTopLeft(event->pos());
      rubberBandRect.setBottomRight(event->pos());
   }

   update();
}

void Plotter::wheelEvent(QWheelEvent *event)
{
   if (curve.isEmpty())
   {
      return;
   }

   if (curve.size()== 1 )
   {
      return;
   }

   if (!inSideAxes(event->pos()))
   {
      return;
   }


   int numDegrees = event->delta();
   int k = 0;

   if (numDegrees > 0)
   {
      wheelValue++;
   }
   else
   {
      wheelValue--;
   }

   if (wheelValue < 0)
   {
      wheelValue = 0;
   }

   for (int i = 0 ; i < curve.size(); ++i)
   {
      // should only do this if it is curve of data type
      // if curve.Type == "Data";
      curve[i]->offset = wheelValue * k * zoomStack[currentZoom].spanY()* 0.033;
      k++;
   }

   doAutoScaleY();
   zoomStack[currentZoom].adjust();
   update();
}

void Plotter::mouseDoubleClickEvent(QMouseEvent *event)
{
   if (curve.isEmpty())
   {
      return;
   }

   if (!inSideAxes(event->pos()))
   {
      return;
   }

   if (xExploreState && event->button() == Qt::LeftButton)
   {
      if (inSideAxes(event->pos()) )
      {
         QPointF mouseAxesPos = pixelToAxes(event->pos());

         if (channelExplorerSpinBox->value() ==  0)
         {
            emit renderChannel(mouseAxesPos.x());  // should really return mouseAxesPos here and delete selChannel stuff
         }
         else
         {
            emit renderSum(mouseAxesPos.x(), channelExplorerSpinBox->value());
         }

         update();
         emit writeMessage("Double click at " + QString::number(mouseAxesPos.x()) + "," + QString::number(mouseAxesPos.y())+"\n");
      }
      xExploreFreeze = !xExploreFreeze;
   }
}

void Plotter::mouseReleaseEvent(QMouseEvent *)
{
   if (curve.isEmpty())
   {
      return;
   }

   if (!showRubberBand)
   {
      return;
   }

   showRubberBand = false;
   QRect rect = rubberBandRect.normalized();

   if (rect.width() < 10 || rect.height() < 10)
   {
      update();
      return;
   }

   AxisSettings newSettings;
   QPointF p, q, r, qS, rS;
   p = rubberBandRect.bottomLeft();
   q = pixelToAxes(p);
   qS = pixelToAxes(p, true);
   p = rubberBandRect.topRight();
   r = pixelToAxes(p);
   rS = pixelToAxes(r, true);

   if (q.x() < r.x())
   {
      newSettings.minX = q.x() ;
      newSettings.maxX = r.x();
   }
   else
   {
      newSettings.minX = r.x() ;
      newSettings.maxX = p.x();

   }
   if (q.y() < r.y())
   {
      newSettings.minY = q.y() ;
      newSettings.maxY = r.y() ;
      newSettings.minSummedY = zoomStack[currentZoom].minSummedY;
      newSettings.maxSummedY = zoomStack[currentZoom].maxSummedY;
   }
   else
   {
      newSettings.minY = r.y() ;
      newSettings.maxY = q.y() ;
      newSettings.minSummedY = zoomStack[currentZoom].minSummedY ;
      newSettings.maxSummedY = zoomStack[currentZoom].maxSummedY;
   }

   newSettings.autoScaleX = false;
   newSettings.autoScaleY = false;
   newSettings.autoScaleSummedY = false;

   currentZoom++;
   zoomStack.resize(currentZoom);
   zoomStack.append(newSettings);
   zoomStack[currentZoom].adjust();

   if (sumState)
   {
      emit renderSum(zoomStack[currentZoom].minX,zoomStack[currentZoom].maxX);
   }

   update();
}

QPointF Plotter::pixelToAxes (QPointF pixelPos, bool summed)
{

   QPointF axesPos;

   if (summed)
   {
      axesPos.setY((double) zoomStack[currentZoom].spanSummedY() - (pixelPos.y() - topMargin) /
                   scaleSummedY + zoomStack[currentZoom].minSummedY);
   }
   else
   {
      axesPos.setY((double) zoomStack[currentZoom].spanY() - (pixelPos.y() - topMargin) /
                   scaleY + zoomStack[currentZoom].minY);

   }
   axesPos.setX((double) (pixelPos.x() - leftMargin) / scaleX + zoomStack[currentZoom].minX);

   return axesPos;
}

bool Plotter::inSideAxes(QPointF position)
{
   if (position.x() > axesBox.left() && position.x() < axesBox.right() && position.y() > axesBox.top() && position.y() < axesBox.bottom())
   {
      return(true);
   }
   else
   {
      return(false);
   }
}
/*
void Plotter::setAxisSettings(const AxisSettings &settings)
{
   zoomStack.clear();
   zoomStack.append(settings);
   currentZoom = 1;
   update();
}
*/

void Plotter::zoomIn()
{
   if (currentZoom == zoomStack.size()-1)
   {
      return;
   }

   currentZoom++;
   zoomStack[currentZoom].adjust();

   if (sumState)
   {
      emit renderSum(zoomStack[currentZoom].minX,zoomStack[currentZoom].maxX);
   }

   update();
}

void Plotter::zoomOut()
{
   if (currentZoom == 0)
   {
      return;
   }
   else
   {
      currentZoom--;
   }

   zoomStack[currentZoom].adjust();

   if (sumState)
   {
      emit renderSum(zoomStack[currentZoom].minX,zoomStack[currentZoom].maxX);
   }

   update();
}

void Plotter::gridToggle()
{
   showGridLines = !showGridLines;
   update();
}
void Plotter::setMargins(int left, int right, int top, int bottom)
{
   leftMargin = left;
   rightMargin = right;
   topMargin = top;
   bottomMargin = bottom;
}

void Plotter::setMargins(int value)
{
   leftMargin = value;
   rightMargin = value;
   topMargin = value;
   bottomMargin = value;
}

void Plotter::addSummedCurveData(QVector<double> xData, double *yData, int numberOfBins)
{

   for (int i = 0 ; i < numberOfBins; ++i )
   {
      summedCurve->xData[i] = xData[i];
      summedCurve->yData[i] = yData[i];
   }

   summedCurve->stats();

   if (currentZoom == 0)
   {
      zoomStack[currentZoom].minX = summedCurve->minXData;
      zoomStack[currentZoom].maxX = summedCurve->maxXData;
      zoomStack[currentZoom].minSummedY = summedCurve->minYData;
      zoomStack[currentZoom].maxSummedY = summedCurve->maxYData;
   }
   else
   {
      zoomStack[currentZoom].minSummedY = zoomStack[currentZoom - 1].minSummedY;
      zoomStack[currentZoom].maxSummedY = zoomStack[currentZoom - 1].maxSummedY;
   }
}

void Plotter::copyCurve(Curve *newCurve)
{
   memcpy((void *) &(lastCurve.xData[0]), (void *) &(newCurve->xData[0]), newCurve->xData.count() * sizeof(double));
   memcpy((void *) &(lastCurve.yData[0]), (void *) &(newCurve->yData[0]), newCurve->yData.count() * sizeof(double));
   lastCurve.minXData = newCurve->minXData;
   lastCurve.maxXData = newCurve->maxXData;
   lastCurve.minYData = newCurve->minYData;
   lastCurve.maxYData = newCurve->maxYData;
}

void Plotter::constructCurveData(Curve *newCurve, const QVector <double> &yData, bool parentHold)
{
   if (pixelAddition && parentHold)
   {
      for (int i = 0; i < yData.size(); ++i )
      {
         newCurve->yData[i] = yData[i] + lastCurve.yData[i];
      }
      newCurve->stats();
      copyCurve(newCurve);
   }
   else
   {
      for (int i = 0; i < yData.size(); ++i )
      {
         newCurve->yData[i] = yData[i];
      }
      newCurve->stats();
      if (!pixelAddition)
      {
         copyCurve(newCurve);
      }
   }
}

void Plotter::addCurveData(QVector <double> &yData, bool parentHold)
{
   Curve *newCurve = new Curve();
   newCurve->xData.resize(yData.size());
   newCurve->yData.resize(yData.size());

   for (int i = 0; i < yData.size(); ++i )
   {
      newCurve->xData[i] = (double) i;
   }

   constructCurveData(newCurve, yData, parentHold);

   newCurve->setHold(parentHold);
//   copyLastCurve();
   tidyCurves();
   autoSetCurveColors(newCurve);
   curve.push_back(newCurve);

   if (zoomStack[currentZoom].autoScaleX && zoomStack[currentZoom].autoScaleY)
   {
      doAutoScale();  // is this necessary as a function
   }

   if (zoomStack[currentZoom].autoScaleY && !zoomStack[currentZoom].autoScaleX)
   {
      doAutoScaleY();
   }

   zoomStack[currentZoom].adjust();
}

void Plotter::addCurveData(QVector <double> &xData, QVector <double> &yData, bool parentHold)
{

   if (xData.size() != yData.size())
   {
      return;
   }

   Curve *newCurve = new Curve();
   newCurve->xData.resize(yData.size());
   newCurve->yData.resize(yData.size());
   for (int i = 0 ; i < yData.size(); ++i )
   {
      newCurve->xData[i] = xData[i];
   }

   constructCurveData(newCurve, yData, parentHold);

   newCurve->setHold(parentHold);
//   copyLastCurve();
   tidyCurves();
   autoSetCurveColors(newCurve);
   curve.push_back(newCurve);

   if (zoomStack[currentZoom].autoScaleX && zoomStack[currentZoom].autoScaleY)
   {
      doAutoScale();  // is this necessary as a function
   }

   if (zoomStack[currentZoom].autoScaleY && !zoomStack[currentZoom].autoScaleX)
   {
      doAutoScaleY();
   }

   zoomStack[currentZoom].adjust();
   // temporarily added this for Kiethley purposes
   this->update();
}

void Plotter::addCurveData(const QVector <double> &xData, const QVector <double> &yData,
                           QString imageName, QPoint p, QColor color, bool parentHold)
{
   if (xData.size() != yData.size())
   {
      return;
   }

   Curve *newCurve = new Curve();
   newCurve->xData.resize(yData.size());
   newCurve->yData.resize(yData.size());

   for (int i = 0 ; i < yData.size(); ++i )
   {
      newCurve->xData[i] = xData[i];
   }

   constructCurveData(newCurve, yData, parentHold);
   newCurve->setHold(parentHold);

//   copyLastCurve();
   tidyCurves();
   autoSetCurveColors(newCurve);
   newCurve->legends(imageName, p, color);
   curve.push_back(newCurve);

   if (zoomStack[currentZoom].autoScaleX && zoomStack[currentZoom].autoScaleY)
   {
      doAutoScale();  // is this necessary as a function
   }

   if (zoomStack[currentZoom].autoScaleY && !zoomStack[currentZoom].autoScaleX)
   {
      doAutoScaleY();
   }

   zoomStack[currentZoom].adjust();
   // temporarily added this for Kiethley purposes
   this->update();
}

void Plotter::setAxesAccepted()
{
   setAxes();
}

void Plotter::setAxesRejected()
{
}

void Plotter::setAxesApplied()
{
   setAxes();
}

void Plotter::displayLegendsFinished(int code)
{
   displayLegendsAction->setChecked(false);
}

void Plotter::handlePixelAdditionChanged(bool pixelAddition)
{
   this->pixelAddition = pixelAddition;
}

void Plotter::setAxes()
{
   int zoomStackSize = zoomStack.size();

   if (zoomStackSize > 1)
   {
      zoomStack.remove(1, (zoomStackSize - 1));
      currentZoom = 0;
   }

   if (!setAxesDialog->getAutoScale())
   {
      AxisSettings newSettings(setAxesDialog->getMinX(), setAxesDialog->getMaxX(),
                               setAxesDialog->getMinY(), setAxesDialog->getMaxY(),
                               setAxesDialog->getMinSummedY(), setAxesDialog->getMaxSummedY());
      zoomStack.append(newSettings);
      currentZoom++;
   }

   zoomStack[currentZoom].adjust();
   update();
}

int Plotter::autoSetCurveColors(Curve *newCurve)
{
   colorIndex++;
   if (colorIndex >= colors.length()) // Re-cycle colours when go off the end
   {
      colorIndex = 0;
   }
   newCurve->curvePen.setBrush(colors.at(colorIndex));

   return colorIndex;
}

void Plotter::yLockToggle()
{
   zoomStack[currentZoom].autoScaleY = !zoomStack[currentZoom].autoScaleY;

   if (zoomStack[currentZoom].autoScaleY)
   {
      doAutoScaleY();
   }
   update();

}

void Plotter::holdPlot()
{
   holdState = !holdState;
   update();
}

void Plotter::tidyCurves()
{
   for (int i = 0 ; i < curve.size(); ++i)
   {
      if (!curve[i]->isHeld())
      {
         curve.remove(i);
         colorIndex--;
      }
   }
}

QMainWindow *Plotter::getMainWindow()
{
   return plotWindow;
}

void Plotter::clearPlot()
{
   curve.clear();
   curve.resize(0);
   colorIndex = 0;
   displayLegendsDialog->clearLegends();
   update();
}

void Plotter::exploreToggle()
{
   xExploreState = ! xExploreState;
   if (xExploreState)
   {
      xExploreFreeze = false;
      sumState = false;
   }
   update();
}

void Plotter::sumToggle()
{
   sumState = ! sumState;
   if (sumState)
   {
      int n1 = (int) floor(zoomStack[currentZoom].minX);
      int n2 = (int) ceil(zoomStack[currentZoom].maxX);
      emit renderSum(n1,n2);
      xExploreState = false;
   }
   update();
}

void Plotter::doAutoScale()
{
   if (zoomStack[currentZoom].autoScaleX)
   {
      zoomStack[currentZoom].minX = curve.last()->minXData;// - curve[0].maxXData*0.01;
      zoomStack[currentZoom].maxX = curve.last()->maxXData;
   }
   if (zoomStack[currentZoom].autoScaleY)
   {
      zoomStack[currentZoom].minY = curve.last()->minYData;// - curve[0].maxYData*0.01;
      zoomStack[currentZoom].maxY = curve.last()->maxYData*1.05;
   }
   doAutoScaleY();
}

void Plotter::doAutoScaleY()
{
   double minY,maxY;
   // WARNING
   // this will break when we have real x values - must be transformed to iterator values
   //int minX = (int) floor(zoomStack[currentZoom].minX);
   //int maxX = (int) floor(zoomStack[currentZoom].maxX);

   curve[0]->stats(zoomStack[currentZoom].minX,zoomStack[currentZoom].maxX);  // should be able to comment this line out
   minY = curve[0]->minYData;
   maxY = curve[0]->maxYData;
   for (int i = 1 ; i < curve.size() ; ++i)
   {
      curve[i]->stats(zoomStack[currentZoom].minX,zoomStack[currentZoom].maxX);
      if (curve[i]->minYData < minY)
      {
         minY = curve[i]->minYData;
      }
      if (curve[i]->maxYData > maxY)
      {
         maxY = curve[i]->maxYData;
      }
   }
   zoomStack[currentZoom].minY = minY;
   zoomStack[currentZoom].maxY = maxY*1.05;
}


void Plotter::CESpinBoxCallBack()
{
   if (channelExplorerSpinBox->value() ==  0)
   {
      emit renderChannel(mouseAxesPos.x());  // should really return mouseAxesPos here and delete selChannel stuff
   }
   else
   {
      emit renderSum(mouseAxesPos.x(), channelExplorerSpinBox->value());
   }
   update();
}

void Plotter::BRSpinBoxCallBack()
{
   /*
    if (channelExplorerSpinBox->value() ==  0)
        emit renderChannel(mouseAxesPos.x());  // should really return mouseAxesPos here and delete selChannel stuff
    else
        emit renderSum(mouseAxesPos.x(), channelExplorerSpinBox->value(), backgroundRemoveSpinBox);
    update();
    */
}

void Plotter::showContextMenu(const QPoint &pos)
{
   QPoint globalPos = plotWindow->mapToGlobal(pos);

   QAction* selectedItem = plotterMenu.exec(globalPos);
   if (selectedItem)
   {
      if (selectedItem->text() == "Set Axes...")
      {
         setAxesDialog->initializeAxes(zoomStack[currentZoom].minX,
                                       zoomStack[currentZoom].maxX,
                                       zoomStack[currentZoom].minY,
                                       zoomStack[currentZoom].maxY,
                                       zoomStack[currentZoom].minSummedY,
                                       zoomStack[currentZoom].maxSummedY);
         setAxesDialog->show();
      }
      else
      {
         if (displayLegendsAction->isChecked())
         {
            displayLegendsDialog->show();
         }
         else
         {
            displayLegendsDialog->hide();
         }
      }
   }
}

void Plotter::testDevelopment()
{
   writeMessage("This is a test message from Plotter::testDevelopment() :" + QString::number(123456789));
}

void Plotter::updatePlotter(QPoint p, bool wasDoubleClicked)
{
   if (wasDoubleClicked && !isVisible())
   {
      // This used to make the plot visible again. Needs new version since tabs added. Restore asap.
   }

   if (wasDoubleClicked && !isHeld() && !MainViewer::instance()->isHeld())
   {
//      copyLastCurve();
      clearPlot();
   }

   Slice *slice = DataModel::instance()->getActiveSlice();
   bool sizeIsGood =  (p.x() > -1 &&
                       p.y() > -1 &&
                       p.x() < slice->getGridSizeX() &&
                       p.y() < slice->getGridSizeY());

   if (sizeIsGood)
   {
      title(slice->objectName() + " coordinates (" + QString::number(p.x()+1) + "," + QString::number(p.y()+1) + ")" );
      addSummedCurveData(slice->getXData(0, 0),slice->getSummedImageY(), slice->getNumberOfBins());
      addCurveData(slice->getXData(p.x(), p.y()), slice->getYData(p.x(), p.y()),
                   slice->objectName(), p, colors.at(colorIndex), wasDoubleClicked);
      if (wasDoubleClicked)
      {
         displayLegendsDialog->addLegend(slice->objectName(), p, colors.at(colorIndex));
         if (pixelAddition)
         {
            emit pixelAdded(QString::number(p.x()+1) + "," + QString::number(p.y()+1));
         }
         else
         {
            emit pixelAdded(NULL);
            emit pixelAdded(QString::number(p.x()+1) + "," + QString::number(p.y()+1));
         }
      }
   }
   else
   {
      this->tidyCurves();
   }
   this->update();
}

void Plotter::updatePlotter(QVector<double> xData, double *summedCurveY, int numberOfBins)
{
   tidyCurves();
   addSummedCurveData(xData, summedCurveY, numberOfBins);
   this->update();
   MainViewer::instance()->getRenderArea()->setMouseEnabled(true);
}

void Plotter::updatePlotter()
{
   this->clearPlot();
}

void Plotter::updatePlotter(QVector <QPoint> &pixelList, bool wasDoubleClicked)
{
   // this function is not currently used
   this->clearPlot();
   for (int i = 0 ; i < pixelList.size(); ++i)
   {
      updatePlotter(pixelList[i], wasDoubleClicked);
   }
   if (pixelList.isEmpty())
   {
      this->update();
   }
}

