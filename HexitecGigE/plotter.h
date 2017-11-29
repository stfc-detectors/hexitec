/*
 Plotter class for 2Easy code
 - under construction

 S D M Jacques 02 May 2011
*/
#ifndef PLOTTER_H
#define PLOTTER_H

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QVector>
#include <QToolButton>
#include <QSpinBox>
#include <QLabel>
#include <QMainWindow>
#include <QPointF>
#include <QPoint>
#include <QMenu>
#include <QToolTip>
#include <QVector>

#include "axissettings.h"
#include "curve.h"
#include "setaxesdialog.h"
#include "displaylegendsdialog.h"
#include "inifile.h"

class Plotter : public QWidget
{
   Q_OBJECT

private:

public:
   explicit Plotter(QWidget *parent = 0);
   bool isDynamic() {return dynamicState;}
   void setDynamicStateOn() {dynamicState = true;}
   void setDynamicStateOff() {dynamicState = false;}
   QVector <Curve *> curve;  // privatise ??
   Curve *summedCurve;
   Curve lastCurve;
   QVector <AxisSettings> zoomStack;   // privatise ??
   int currentZoom;  // privatise ??
   //    void setAxisSettings(const AxisSettings &settings);   // privatise ??
   void setLeftMargin(int value) { leftMargin = value; }
   void setRightMargin(int value) { rightMargin = value; }
   void setTopMargin(int value) { topMargin = value; }
   void setBottomMargin(int value) { bottomMargin = value; }
   void setMargins(int);
   void setMargins(int, int, int, int);
   void axesOn() { showAxes = true; }
   void axesOff() { showAxes = false; }
   void gridOn() { showGridLines = true; }
   void gridOff() { showGridLines = false; }
   void zoomOn() { zoomState = true ;}
   void zoomOff() { zoomState = false ;}
   void xExploreOn() {xExploreState = true; xExploreFreeze = false; sumState = false;}
   void xExploreOff() {xExploreState = false; xExploreFreeze = true;}
   void sumOn() {sumState = true; xExploreState = false;}
   void sumOff() {sumState = false;}
   bool inSideAxes(QPointF);
   bool isHeld() { return holdState; }
   void holdOn() { holdState = true; }
   void holdOff() { holdState = false; }
   void title(QString value) { plotTitle = value;}
   void xLabel(QString value) { xPlotLabel = value; }
   void yLabel(QString value) { yPlotLabel = value; }
   void summedYLabel(QString value) { summedYPlotLabel = value; }
   int autoSetCurveColors(Curve *newCurve);
   void tidyCurves();

   QMainWindow *getMainWindow();

   QString title() { return plotTitle; }
   QString xLabel() { return xPlotLabel; }
   QString yLabel() { return yPlotLabel; }
   QString summedYLabel() { return summedYPlotLabel; }


protected:
   void paintEvent(QPaintEvent *event);
   void mouseMoveEvent(QMouseEvent *event);
   void mousePressEvent(QMouseEvent *event);
   void mouseDoubleClickEvent(QMouseEvent *event);
   void mouseReleaseEvent(QMouseEvent *);
   void wheelEvent(QWheelEvent *event);

signals:
   void renderChannel(double);
   void renderSum(double, int);
   void renderSum();
   void renderSum(double, double);
   void writeMessage(QString);
   void writeWarning(QString);
   void writeError(QString);
   void pixelAdded(QString);

public slots:
   void zoomIn();
   void zoomOut();
   void yLockToggle();
   void gridToggle();
   void holdPlot();
   void clearPlot();
   void exploreToggle();
   void sumToggle();
   void testDevelopment();

   void updatePlotter();
   void updatePlotter(QPoint, bool);
   void updatePlotter(QVector <QPoint> &, bool);
   void updatePlotter(QVector<double>, double *, int);
   void addSummedCurveData(QVector<double>, double *, int);
   void addCurveData(QVector <double> &, bool);
   // void addCurveData(QVector <double> &, QVector <double> &);
   void addCurveData(QVector <double> &, QVector <double> &, bool);
   void addCurveData(const QVector <double> &xData, const QVector <double> &yData,
                     QString imageName, QPoint p, QColor color, bool parentHold);
   void paintCurve(Curve *curve, QPainterPath *path);
   void paintSummedCurve(QPainterPath *path);
   void setAxesAccepted();
   void setAxesRejected();
   void setAxesApplied();
   void displayLegendsFinished(int code);
   void handlePixelAdditionChanged(bool);

private:
   enum { defaultMarginValue = 55 };
   bool dynamicState;
   bool showAxes;
   bool zoomState;
   bool showGridLines;
   bool showRubberBand;
   bool mouseButtonPressed;
   bool mouseDoubleClicked;
   bool xExploreState;
   bool xExploreFreeze;
   bool holdState;
   bool sumState;
   bool spSumState;
   QString *toolTip;
   bool pixelAddition;

   void copyCurve(Curve *newCurve);
   void constructCurveData(Curve * newCurve, const QVector <double> &yData, bool parentHold);
   QRect rubberBandRect;
   QRectF axesBox;
   int currentCurve;
   QString plotTitle;
   QString xPlotLabel;
   QString yPlotLabel;
   QString summedYPlotLabel;
   QPoint toolTipPoint;
   // pixel definitions
   int leftMargin;
   int rightMargin;
   int topMargin;
   int bottomMargin;
   double scaleX;
   double scaleY;
   double scaleSummedY;
   int maxBins;

   QPointF pixelToAxes (QPointF, bool summed = false);
   QPointF axesToPixel (QPointF);
   QPointF mousePosition;
   QPointF mouseAxesPos;

   void setAxes();
   void doAutoScale();
   void doAutoScaleY();

   int numberOfSumPoints;
   int numberLeftPoints;
   int numberRightPoints;

   QSpinBox *channelExplorerSpinBox;
//   QSpinBox *backgroundRemoveSpinBox;

   int wheelValue;
   QMainWindow *plotWindow;
   QToolBar *createToolBar();

   QList<QColor> colors;
   int colorIndex;
   QMenu plotterMenu;
   QAction *setAxesAction;
   QAction *displayLegendsAction;
   QAction *displayPlotValueAction;
   SetAxesDialog *setAxesDialog;
   DisplayLegendsDialog *displayLegendsDialog;
   IniFile *twoEasyIniFile;

private slots:
   void CESpinBoxCallBack();
   void BRSpinBoxCallBack();
   void showContextMenu(const QPoint&);
};

#endif // PLOTTER_H
