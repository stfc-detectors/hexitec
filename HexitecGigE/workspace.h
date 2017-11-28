#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QMainWindow>
#include <QTreeView>
#include <QMenuBar>
#include <QAction>
#include <QString>
#include <QObject>

#include "datamodel.h"
#include "getvaluedialog.h"
//#include "matlab.h"
//#include "matlabvariable.h"
#include "mainviewer.h"

class Workspace : public QWidget
{
   Q_OBJECT

public:
   Workspace();
   QMainWindow *getMainWindow();

public slots:
//   void matlabStatus(bool status);

private slots:
   void addConstant();
   void addSlices();
   void deleteSlice();
//   void exportSlice();
//   void importArray();
//   void maskSlice();
   void multiplySlice();
   void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
//   void refresh();

signals:
   void exportSlice(Slice *);
   void initializeSlice(Slice *);
   void removeSlice(Slice *);
   void writeError(QString);
   void writeMessage(QString);

private:
   QAction *addConstantAction;
   QAction *addSlicesAction;
   DataModel *dataModel;
   QAction *deleteAction;
//   QAction *exportAction;
   QAction *importAction;
   QMainWindow *mainWindow;
   QAction *maskAction;
//   bool matlabAvailable;
   QAction *multiplyAction;
   QTreeView *workspaceTree;
   int slicesSelected;
   int arraysSelected;

   int countSlicesSelected();
   int countArraysSelected();
   void createFileMenu();
   void createOpsMenu();
};

#endif // WORKSPACE_H
