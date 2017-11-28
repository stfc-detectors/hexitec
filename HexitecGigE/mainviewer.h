#ifndef MAINVIEWER_H
#define MAINVIEWER_H

#include <QWidget>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QtGui>

#include "renderarea.h"
//#include "matlabvariable.h"
#include "sarray.h"

// MainViewer is a wrapper for the main renderArea.
class MainViewer : public QMainWindow
{
   Q_OBJECT

public:
   static MainViewer *instance();
   RenderArea *getRenderArea();
   AxisSettings getAxes();
   QToolBar *createToolbar();
   void showNewActiveSlice();
   bool isHeld();
//   void showMatlabArray(MatlabVariable *ma);

protected:
   void dragEnterEvent(QDragEnterEvent *event);
   void dragMoveEvent(QDragMoveEvent *event);
   void dragLeaveEvent(QDragLeaveEvent *event);
   void dropEvent(QDropEvent *event);

private:
   MainViewer();
   static MainViewer *mVInstance;
   RenderArea *renderArea;
   QStringList interpretMimeData(const QMimeData *mimeData);

signals:
   void writeMessage(QString message);
   void readFiles(QStringList fileNames);

public slots:
   void renderChannel(double value);
   void renderSum(double minX, double maxX);
   void renderSum(double xVal, int range);
   void activeSliceChanged(RenderArea *selected);
};

#endif // MAINVIEWER_H
