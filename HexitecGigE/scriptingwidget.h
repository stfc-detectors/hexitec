#ifndef SCRIPTINGWIDGET_H
#define SCRIPTINGWIDGET_H

//#include <QtGui>
#include <QWidget>
#include <QtScript>
#include <QTextBrowser>
#include <QPushButton>
#include <QFileDialog>
#include <QList>
#include <QListIterator>
#include <QMainWindow>
//#include "dummymotor.h"
//#include "matlab.h"
#include "scriptrunner.h"
#include "reservable.h"
#include "reservation.h"
#include "objectreserver.h"

namespace Ui {
class ScriptingWidget;
}

class ScriptingWidget : public QWidget
{
   Q_OBJECT

public:
   static ScriptingWidget *instance();
   ScriptRunner *getScriptRunner();
   ~ScriptingWidget();
   QMainWindow *getMainWindow();
   bool eventFilter(QObject *object, QEvent *event);
   void runInitScript();
   bool loadScript(QString fileName);
   void saveScript(QString fileName);

signals:
   void addObject(QObject *object, bool, bool);

public slots:
   void scriptDone(bool success);

protected slots:
   void mainContextMenuEvent(QPoint point);
   void lineContextMenuEvent(QPoint point);
   void editorContextMenuEvent(QPoint point);
   void outputContextMenuEvent(QPoint point);

private slots:
   void executeScript();
   void executeLine();
   void loadScript();

   void saveScript();
//   void setMatlab();
   void setScripting();
   void toggle();
   void clearEditor();
   void clearOutput();
   void updateOutput(QString message);

private:
   ScriptingWidget(QWidget *parent = 0);
   static ScriptingWidget *swInstance;
   ScriptRunner *scriptRunner;
   QMainWindow *scriptingMainWindow;
   static const int MATLAB = 1;
   static const int SCRIPTING = 2;
   int destination;
   Ui::ScriptingWidget *ui;
   void scriptError();
//   matlab *matlab;
   QList<QString> commands;
   QList<QString>::iterator commandIterator;
   void executeScript(QString script);
   void sendCommands(QString commands);
   void setModeFromFile(QString fileName);
   QString scriptsDir;
   bool scriptIsRunning;
   void setScriptIsRunning(bool scriptIsRunning);
   QList<QString> sections;
   void constructSections(QString string);
   void processNextSection();
   bool isInitScript;
   Reservation reservation;
//   QAction *matlabAction;
   QAction *scriptingAction;
   QAction *clearAction;
   void createActions();
   QMenu *mainContextMenu;
   QMenu *scriptEditorContextMenu;
   QMenu *scriptLineContextMenu;
   QMenu *scriptOutputContextMenu;
   void createContextMenus();
   QDockWidget *scriptingDockWidget;
   QString removeComments(QString originalScript);
   QString reservables(QString originalScript, QList<QObject *> noReservationsObjects);
};

#endif // SCRIPTINGWIDGET_H
