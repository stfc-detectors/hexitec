#ifndef THUMBVIEWER_H
#define THUMBVIEWER_H

#include <QWidget>
#include <QVector>
#include <QMainWindow>

#include "datamodel.h"
#include "renderarea.h"
#include "slice.h"

class ThumbViewer : public QWidget
{
   Q_OBJECT
public:
   explicit ThumbViewer(QWidget *parent = 0);
   QVector <RenderArea *> thumbNails;
   void setCellSize(int);
   void setCellSize(int, int);
   void setOffset(int, int);
   bool autoScaleZ() {return(showColorBars);}
   void addSlice(Slice *slice);
   void deleteSlice(Slice *slice);
   void scaleZAxes(AxisSettings axes);
   QMainWindow *getMainWindow();


signals:
   void activeSliceChanged(RenderArea *);
   void writeMessage(QString);
   void writeWarning(QString);
   void writeError(QString);

protected:
   void paintEvent(QPaintEvent *event);
   void mouseMoveEvent(QMouseEvent *event);
   void mousePressEvent(QMouseEvent *);
   void mouseDoubleClickEvent(QMouseEvent *event);
   void mouseReleaseEvent(QMouseEvent *);
   void wheelEvent(QWheelEvent *event);
   void leaveEvent(QEvent *event);

public slots:
   void zoomIn();
   void zoomOut();
   void colorBarsToggle();
   void cycleColorMap();
   void testDevelopment();
   void renderChannel(double value);
   void renderSum(double minX, double maxX);
   void renderSum(double xVal, int range);

private:
   bool sameCellSizes;
   int thumbUnderMouse;
   int activeSlice;
   int offsetX;
   int offsetY;
   int thumbOffsetX;
   int thumbOffsetY;
   int cellSizeX;
   int cellSizeY;
   QVector <QRect> thumbRect;
   bool showColorBars;
   QMainWindow *thumbWindow;
   QToolBar *createToolBar();

};

#endif // THUMBVIEWER_H
