#include "workspace.h"

// Constructors
Workspace::Workspace()
{
   mainWindow = new QMainWindow();

   workspaceTree = new QTreeView();
   workspaceTree->setSelectionBehavior(QAbstractItemView::SelectRows);
   workspaceTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
   dataModel = DataModel::instance();
   workspaceTree->setModel(dataModel);

   connect(workspaceTree->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
           this, SLOT(selectionChanged(QItemSelection,QItemSelection)));

   mainWindow->setCentralWidget(workspaceTree);

   createOpsMenu();
}

// Other methods
void Workspace::addConstant()
{
   GetValueDialog *gvd = GetValueDialog::instance();
   gvd->setLabel("Add:");
   gvd->exec();

   if (gvd->result() == QDialog::Accepted)
   {
      QModelIndexList indexList = workspaceTree->selectionModel()->selectedRows(0);
      Slice *newSlice = DataModel::instance()->getSlice(indexList[0])->add(gvd->getDoubleValue());
      emit initializeSlice(newSlice);
   }
}

void Workspace::addSlices()
{
   QModelIndexList indexList = workspaceTree->selectionModel()->selectedRows(0);

   try
   {
      Slice *newSlice =
            DataModel::instance()->getSlice(indexList[0])->add(DataModel::instance()->getSlice(indexList[1]));

      emit initializeSlice(newSlice);
   }
   catch (QString message)
   {
      emit writeError(message);
   }
}

int Workspace::countArraysSelected()
{
   QModelIndexList indexList = workspaceTree->selectionModel()->selectedRows(0);
   int count = 0;
//   for (int i = 0; i < indexList.size(); i++)
//   {
//      if (DataModel::instance()->getMatlabArray(indexList[i]) != NULL)
//      {
//         count++;
//      }
//   }
   // If count is not equal to the list size then something other than a slice is selected - for example a volume -
   // and we indicate this by setting count to -1 so that the ops menu items will be disabled.
   if (count != indexList.size())
   {
      count = -1;
   }
   return count;
}

int Workspace::countSlicesSelected()
{
   QModelIndexList indexList = workspaceTree->selectionModel()->selectedRows(0);
   int count = 0;
   for (int i = 0; i < indexList.size(); i++)
   {
      if (DataModel::instance()->getSlice(indexList[i]) != NULL)
      {
         count++;
      }
   }
   // If count is not equal to the list size then something other than a slice is selected - for example a volume -
   // and we indicate this by setting count to -1 so that the ops menu items will be disabled.
   if (count != indexList.size())
   {
      count = -1;
   }
   return count;
}

void Workspace::createOpsMenu()
{
   QMenu *opsMenu = mainWindow->menuBar()->addMenu("&Ops");

   addConstantAction = new QAction("&Add(Constant)", opsMenu);
   addConstantAction->setEnabled(false);
   connect(addConstantAction, SIGNAL(triggered()), this, SLOT(addConstant()));
   opsMenu->addAction(addConstantAction);

   addSlicesAction = new QAction("&Add(Slices)", opsMenu);
   addSlicesAction->setEnabled(false);
   connect(addSlicesAction, SIGNAL(triggered()), this, SLOT(addSlices()));
   opsMenu->addAction(addSlicesAction);

   deleteAction = new QAction("&Delete", this);
   deleteAction->setEnabled(false);
   connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteSlice()));
   opsMenu->addAction(deleteAction);

//   exportAction = new QAction(QIcon(":/images/exportToMatlab.png"), "&Export", opsMenu);
//   exportAction->setEnabled(false);
//   connect(exportAction, SIGNAL(triggered()), this, SLOT(exportSlice()));
//   opsMenu->addAction(exportAction);

//   importAction = new QAction(QIcon(":/images/importFromMatlab.png"), "&Import", opsMenu);
//   importAction->setEnabled(false);
//   connect(importAction, SIGNAL(triggered()), this, SLOT(importArray()));
//   opsMenu->addAction(importAction);

//   maskAction = new QAction("&Mask", opsMenu);
//   maskAction->setEnabled(false);
//   connect(maskAction, SIGNAL(triggered()), this, SLOT(maskSlice()));
//   opsMenu->addAction(maskAction);

   multiplyAction = new QAction("&Multiply", opsMenu);
   multiplyAction->setEnabled(false);
   connect(multiplyAction, SIGNAL(triggered()), this, SLOT(multiplySlice()));
   opsMenu->addAction(multiplyAction);

//   QAction *refreshAction = new QAction("&Refresh", opsMenu);
//   connect(refreshAction, SIGNAL(triggered()), this, SLOT(refresh()));
//   opsMenu->addAction(refreshAction);
}

void Workspace::deleteSlice()
{
   QModelIndexList indexList = workspaceTree->selectionModel()->selectedRows(0);
   QModelIndex index = indexList[0];

   emit removeSlice(DataModel::instance()->getSlice(index));
}

//void Workspace::exportSlice()
//{
//   QModelIndexList indexList = workspaceTree->selectionModel()->selectedRows(0);
//   DataModel::instance()->getSlice(indexList[0])->sendToMatlab();
//}

//void Workspace::importArray()
//{
//   QModelIndexList indexList = workspaceTree->selectionModel()->selectedRows(0);
//   MatlabVariable *ma = DataModel::instance()->getMatlabArray(indexList[0]);
//   ma->importData();
//   MainViewer::instance()->showMatlabArray(ma);
//}

QMainWindow *Workspace::getMainWindow()
{
   return mainWindow;
}

//void Workspace::maskSlice()
//{
//   GetValueDialog *gvd = GetValueDialog::instance();
//   gvd->setLabel("Mask with MATLAB variable: ");
//   gvd->exec();

//   if (gvd->result() == QDialog::Accepted)
//   {
//      double *mask = matlab::instance()->getArray(gvd->getStringValue());
//      QModelIndexList indexList = workspaceTree->selectionModel()->selectedRows(0);
//      Slice *newSlice = DataModel::instance()->getSlice(indexList[0])->veil(mask);
//      emit initializeSlice(newSlice);
//   }
//}

//void Workspace::matlabStatus(bool status)
//{
//   matlabAvailable = status;
//   exportAction->setEnabled(matlabAvailable && (slicesSelected == 1));
//   importAction->setEnabled(matlabAvailable && (arraysSelected == 1));
//}

void Workspace::multiplySlice()
{
   GetValueDialog *gvd = GetValueDialog::instance();
   gvd->setLabel("Multiply by:");
   gvd->exec();

   if (gvd->result() == QDialog::Accepted)
   {
      QModelIndexList indexList = workspaceTree->selectionModel()->selectedRows(0);
      Slice *newSlice = DataModel::instance()->getSlice(indexList[0])->multiply(gvd->getDoubleValue());
      emit initializeSlice(newSlice);
   }
}

void Workspace::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
   slicesSelected = countSlicesSelected();
   arraysSelected = countArraysSelected();

   addConstantAction->setEnabled(slicesSelected == 1);
   addSlicesAction->setEnabled(slicesSelected == 2);
   deleteAction->setEnabled(slicesSelected == 1);
//   exportAction->setEnabled(matlabAvailable && (slicesSelected == 1));
//   importAction->setEnabled(matlabAvailable && (arraysSelected == 1));
   maskAction->setEnabled(slicesSelected == 1);
   multiplyAction->setEnabled(slicesSelected == 1);
}
/*
  Refreshes the list of objects by adding MatlabVariables.
  */
//void Workspace::refresh()
//{
//   DataModel::instance()->removeAllMatlabVariables();

//   QStringList vars = matlab::instance()->listVariables();
//   for (int i = 0; i < vars.size(); i++)
//   {
//      try
//      {
//         new MatlabVariable(vars.at(i));
//      }
//      catch (QString message)
//      {
//         // This is not really an error, it just means that the variable already exists in the
//         // workspace so we do nothing. At the moment we delete all matlab variables anyway so
//         // this should not happen.
//      }
//   }
//}
