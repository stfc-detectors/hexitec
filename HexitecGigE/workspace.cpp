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
      if (DataModel::instance()->getSlice(indexList[i]) != nullptr)
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

   multiplyAction = new QAction("&Multiply", opsMenu);
   multiplyAction->setEnabled(false);
   connect(multiplyAction, SIGNAL(triggered()), this, SLOT(multiplySlice()));
   opsMenu->addAction(multiplyAction);
}

void Workspace::deleteSlice()
{
   QModelIndexList indexList = workspaceTree->selectionModel()->selectedRows(0);
   QModelIndex index = indexList[0];

   emit removeSlice(DataModel::instance()->getSlice(index));
}

QMainWindow *Workspace::getMainWindow()
{
   return mainWindow;
}

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
