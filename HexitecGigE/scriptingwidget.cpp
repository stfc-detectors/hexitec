#include <QEvent>
#include <QShortcut>
#include <QDockWidget>
#include <QMenu>
#include <QKeyEvent>

#include "scriptingwidget.h"
#include "ui_scriptingwidget.h"

ScriptingWidget *ScriptingWidget::swInstance = NULL;

ScriptingWidget *ScriptingWidget::instance()
{
   if (swInstance == NULL)
   {
      swInstance = new ScriptingWidget();
   }
   return swInstance;
}

ScriptingWidget::ScriptingWidget(QWidget *parent) :
   QWidget(parent),
   ui(new Ui::ScriptingWidget)
{
   ui->setupUi(this);
   setLayout(ui->gridLayout);
   ui->gridLayout->setContentsMargins(5, 5, 5, 5);

   scriptsDir = "../scripts";

   scriptRunner = new ScriptRunner();
   connect(scriptRunner, SIGNAL(updateOutput(QString)), this, SLOT(updateOutput(QString)));
   connect(scriptRunner, SIGNAL(scriptDone(bool)), this, SLOT(scriptDone(bool)));

   connect(this, SIGNAL(addObject(QObject*, bool, bool)), ScriptingWidget::getScriptRunner(),
           SLOT(addObject(QObject*, bool, bool)));

   connect(ui->executeButton, SIGNAL(clicked()), this, SLOT(executeScript()));
   connect(ui->loadButton, SIGNAL(clicked()), this, SLOT(loadScript()));
   connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveScript()));
   connect(ui->scriptLine, SIGNAL(returnPressed()), this, SLOT(executeLine()));

   createActions();
   createContextMenus();

//   matlab = matlab::instance();
//   connect(matlab, SIGNAL(updateOutput(QString)), this, SLOT(updateOutput(QString)));
//   connect(matlab, SIGNAL(scriptDone(bool)), this, SLOT(scriptDone(bool)));

   // Since the matlabAction and the scriptingAction have shortcuts set none of this section should be necessary
   ui->scriptEditor->installEventFilter(this);
   ui->scriptLine->installEventFilter(this);
   ui->scriptOutput->installEventFilter(this);
//   QShortcut *qscOne = new QShortcut(QKeySequence(tr("Alt+M", "matlab")), this);
//   connect(qscOne, SIGNAL(activated()), this, SLOT(setMatlab()));
   QShortcut *qscTwo = new QShortcut(QKeySequence(tr("Alt+J", "javascript")), this);
   connect(qscTwo, SIGNAL(activated()), this, SLOT(setScripting()));
   QShortcut *qscThree = new QShortcut(QKeySequence(tr("Alt+T", "toggle")), this);
   connect(qscThree, SIGNAL(activated()), this, SLOT(toggle()));
   // End of unnecessary section

   scriptingMainWindow = new QMainWindow();
   scriptingDockWidget = new QDockWidget("Scripting", scriptingMainWindow);
   scriptingDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
   scriptingDockWidget->setWidget(this);
   scriptingMainWindow->addDockWidget(Qt::BottomDockWidgetArea, scriptingDockWidget);

   setScripting();
   scriptIsRunning = false;
}

void ScriptingWidget::runInitScript()
{
   QString initFileName = scriptsDir + "/init.js";
   QFile *scriptInitFile = new QFile(initFileName);

   if (scriptInitFile->exists())
   {
      isInitScript = true;
      loadScript(initFileName);
      executeScript();
   }
}

ScriptingWidget::~ScriptingWidget()
{
   delete ui;
}

QMainWindow *ScriptingWidget::getMainWindow()
{
   return scriptingMainWindow;
}

void ScriptingWidget::editorContextMenuEvent(QPoint point)
{
   scriptEditorContextMenu->exec(ui->scriptEditor->mapToGlobal(point));
}

void ScriptingWidget::lineContextMenuEvent(QPoint point)
{
   scriptLineContextMenu->exec(ui->scriptLine->mapToGlobal(point));
}

void ScriptingWidget::mainContextMenuEvent(QPoint point)
{
   mainContextMenu->exec(this->mapToGlobal(point));
}

void ScriptingWidget::outputContextMenuEvent(QPoint point)
{
   scriptOutputContextMenu->exec(ui->scriptOutput->mapToGlobal(point));
}

void ScriptingWidget::createActions()
{
   clearAction = new QAction(tr("Clear"), this);
   clearAction->setShortcut(QKeySequence(tr("Alt+C")));
   clearAction->setStatusTip(tr("Clear output screen"));
   connect(clearAction, SIGNAL(triggered()), this, SLOT(clearEditor()));

//   matlabAction = new QAction(tr("Matlab"), this);
//   matlabAction->setShortcut(QKeySequence(tr("Alt+M")));
//   matlabAction->setStatusTip(tr("Set destination to matlab"));
//   matlabAction->setCheckable(true);
//   connect(matlabAction, SIGNAL(triggered()), this, SLOT(setMatlab()));

   scriptingAction = new QAction(tr("JavaScript"), this);
   scriptingAction->setShortcut(QKeySequence(tr("Alt+J")));
   scriptingAction->setStatusTip(tr("Set destination to javascript"));
   scriptingAction->setCheckable(true);
   connect(scriptingAction, SIGNAL(triggered()), this, SLOT(setScripting()));
}

void ScriptingWidget::createContextMenus()
{
   mainContextMenu = new QMenu();
//   mainContextMenu->addAction(matlabAction);
   mainContextMenu->addAction(scriptingAction);
   connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(mainContextMenuEvent(QPoint)));

   scriptEditorContextMenu = ui->scriptEditor->createStandardContextMenu();
   scriptEditorContextMenu->addAction(clearAction);
//   scriptEditorContextMenu->addAction(matlabAction);
   scriptEditorContextMenu->addAction(scriptingAction);
   connect(ui->scriptEditor, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(editorContextMenuEvent(QPoint)));

   scriptLineContextMenu = ui->scriptLine->createStandardContextMenu();
//   scriptLineContextMenu->addAction(matlabAction);
   scriptLineContextMenu->addAction(scriptingAction);
   connect(ui->scriptLine, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(lineContextMenuEvent(QPoint)));

   scriptOutputContextMenu = ui->scriptOutput->createStandardContextMenu();
   scriptOutputContextMenu->addAction(clearAction);
   connect(ui->scriptOutput, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(outputContextMenuEvent(QPoint)));
}

bool ScriptingWidget::eventFilter(QObject *object, QEvent *event)
{
   bool eventHandled = false;
   QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

   if (object == ui->scriptEditor && event->type() == QEvent::KeyPress)
   {
     if (keyEvent->key() == Qt::Key_C && keyEvent->modifiers() == Qt::ALT)
      {
         clearEditor();
         eventHandled = true;
      }
   }
   else if (object == ui->scriptOutput && event->type() == QEvent::KeyPress)
   {
      if (keyEvent->key() == Qt::Key_C && keyEvent->modifiers() == Qt::ALT)
      {
         clearOutput();
         eventHandled = true;
      }
   }
   else if (object == ui->scriptLine && event->type() == QEvent::KeyPress)
   {
      if (keyEvent->key() == Qt::Key_Up)
      {
         if (commandIterator != commands.begin() && commands.size() > 0)
         {
            commandIterator--;
            ui->scriptLine->setText(*commandIterator);
         }
         eventHandled = true;
      }
      else if (keyEvent->key() == Qt::Key_Down)
      {
         if (commandIterator != commands.end())
         {
            commandIterator++;
            if (commandIterator != commands.end() && commands.size() > 0)
            {
               ui->scriptLine->setText(*commandIterator);
            }
            else
            {
               ui->scriptLine->setText("");
            }
         }
         eventHandled = true;
      }
   }
   return eventHandled;
}

//void ScriptingWidget::setMatlab()
//{
//   destination = MATLAB;
//   matlabAction->setChecked(true);
//   scriptingAction->setChecked(false);
//   ui->lineEditLabel->setText("   MATLAB>>");
//   ui->lineEditLabel->setStyleSheet("color:orangered;");
//   scriptingDockWidget->setWindowTitle("MATLAB");
//}

void ScriptingWidget::setScripting()
{
   destination = SCRIPTING;
//   matlabAction->setChecked(false);
   scriptingAction->setChecked(true);
   ui->lineEditLabel->setText("JAVASCRIPT>>");
   ui->lineEditLabel->setStyleSheet("color:blue;");
   scriptingDockWidget->setWindowTitle("JavaScript");
}

void ScriptingWidget::toggle()
{
   if (destination == MATLAB)
   {
      setScripting();
   }
//   else
//   {
//      setMatlab();
//   }
}

void ScriptingWidget::clearEditor()
{
   ui->scriptEditor->setText("");
}

void ScriptingWidget::clearOutput()
{
   ui->scriptOutput->setText("");
}

void ScriptingWidget::updateOutput(QString message)
{
   ui->scriptOutput->append(message);
   QTextCursor c = ui->scriptOutput->textCursor();
   c.movePosition(QTextCursor::End);
   ui->scriptOutput->setTextCursor(c);
}

void ScriptingWidget::executeScript()
{
   ui->lastCommand->setText("");
   QString script = ui->scriptEditor->document()->toPlainText();

   executeScript(script);
}

void ScriptingWidget::executeScript(QString script)
{
   bool onlyMATLAB = true;

   constructSections(script);

   reservation = Reservation();
   for (int i = 0; i < sections.size(); i++)
   {
      QString section = sections.at(i);
      QString reservableSection;
      // Variable destination indicates the default destination for the whole file or for the first section if it has
      // no keyword. So if the destination is SCRIPTING then we deal with sections which DON'T start with MATLAB but
      // if the destination is MATLAB we deal with sections which DO start with SCRIPTING

      if ((destination == SCRIPTING) && (!section.startsWith("MATLAB")) ||
          (destination == MATLAB) && (section.startsWith("SCRIPTING")))
      {
         // As soon as we start to try and reserve object we are not in a purely MATLAB script.
         onlyMATLAB = false;


         QList<QObject *> reserveForScriptObjects = scriptRunner->getScriptObjects();
         QList<QObject *> reserveForGuiObjects = scriptRunner->getGuiObjects();
         QList<QObject *> freeForScriptObjects = scriptRunner->getObjects();
         QList<QObject *> noReservationsObjects = scriptRunner->getObjects();

         foreach (QObject *object, reserveForScriptObjects)
         {
            freeForScriptObjects.removeAll(object);
            noReservationsObjects.removeAll(object);
         }

         foreach (QObject *object, reserveForGuiObjects)
         {
            noReservationsObjects.removeAll(object);
         }

         reservableSection = reservables(section, noReservationsObjects);
         Reservation r = ObjectReserver::instance()->reserveForScript(reservableSection, reserveForScriptObjects, freeForScriptObjects);
         reservation = reservation.add(r);
         if (reservation.getMessage().compare("Success") != 0)
         {
            break;
         }
      }
   }

   // The first condition is for a purely MATLAB script - no attempt will have been made to reserve any
   // objects. The second condition is for a script which is at least partly SCRIPTING - all the reservation
   // attempts must have succeeded.
   if (onlyMATLAB || reservation.getMessage().compare("Success") == 0)
   {
      processNextSection();
   }
   else
   {
      updateOutput("Could not reserve all objects, message = " + reservation.getMessage());
      scriptDone(false);
   }

   return;
}

void ScriptingWidget::constructSections(QString string)
{
   QStringList qsl = string.split(QRegExp("SCRIPTING|MATLAB"), QString::SkipEmptyParts);

   for (int i = 0; i < qsl.size(); i++)
   {
      sections.append(string.section(QRegExp("SCRIPTING|MATLAB"), i, i, QString::SectionIncludeLeadingSep));
      sections.replace(i, removeComments(sections.at(i)).trimmed());

   }
}

QString ScriptingWidget::removeComments(QString originalScript)
{
   QStringList qsl = originalScript.split("\n");
   QString scriptNoComments = "";
   QString trimmed = "";
   bool inComment = false;

   foreach (QString s, qsl)
   {
      trimmed = s.trimmed();
      if (trimmed.startsWith("/*"))
      {
         inComment = true;
      }
      if (inComment)
      {
         if (trimmed.endsWith("*/"))
         {
            inComment = false;
         }

      }
      else
      {
         if (!trimmed.startsWith("//"))
         {
            scriptNoComments.append("\n");
            scriptNoComments.append(s);
         }
       }
   }

   return scriptNoComments;
}

QString ScriptingWidget::reservables(QString originalScript, QList<QObject *> noReservationsObjects)
{
   QStringList qsl = originalScript.split("\n");
   QString reservablesScript = "";
   QString trimmed = "";
   bool found = false;

   foreach (QString s, qsl)
   {
       found = false;
       trimmed = s.trimmed();
       foreach (QObject * r, noReservationsObjects)
       {
          QString objectName = r->objectName();
          if ((trimmed.contains(objectName + ".") || trimmed.contains(objectName + "->")))
          {
              found = true;
              break;
          }
       }
       if (!found)
       {
          reservablesScript.append("\n");
          reservablesScript.append(s);
       }
   }

   return reservablesScript;
}
void ScriptingWidget::processNextSection()
{
   if (sections.isEmpty())
   {
      //qDebug() << "Setting scriptIsRunning False and releasing reservables";
      setScriptIsRunning(false);
      ObjectReserver::instance()->release(reservation.getReserved(), "ScriptReserver", false);
   }
   else
   {
      sendCommands(sections.takeFirst());
   }
}

void ScriptingWidget::executeLine()
{
   QString command;
   QLineEdit *scriptLine = ui->scriptLine;

   command = scriptLine->text();
   commands.append(command);

   executeScript(command);
}

/*
  Starts processing of commands by scriptRunner or matlab. These both run
  the commands in a separate thread and send a scriptDone signal when
  finished.
  */
void ScriptingWidget::sendCommands(QString commands)
{
   int localDestination = destination;

   if (commands.startsWith("MATLAB\n"))
   {
      localDestination = MATLAB;
      commands.remove(0, 7);
   }
   else if (commands.startsWith("SCRIPTING\n"))
   {
      localDestination = SCRIPTING;
      commands.remove(0, 10);
   }

   switch (localDestination)
   {
   case SCRIPTING:
//      qDebug() << "sendCommands to SCRIPTING";
      setScriptIsRunning(true);
      scriptRunner->setScript(commands);
      updateOutput("Starting SCRIPTING script");
      scriptRunner->start();
      break;
   case MATLAB:
//      qDebug() << "sendCommands to MATLAB";
      setScriptIsRunning(true);
//      matlab->setScript(commands);
      updateOutput("Starting MATLAB script");
//      matlab->startScript();
      break;
   }
}

void ScriptingWidget::setModeFromFile(QString fileName)
{
   if (fileName.endsWith('m'))
   {
      /*setMatlab()*/;
   }
   else if (fileName.endsWith("js"))
   {
      setScripting();
   }
}

void ScriptingWidget::loadScript()
{
   /* Gets a valid fileName from the user. Appends the file contents into the ScriptEditor.
    */

   QString fileName;
   fileName = QFileDialog::getOpenFileName(this,
                                           tr("Open Script"), scriptsDir, tr("Script Files (*.js;*.m)"));
   if (!fileName.isEmpty())
   {
      if (!loadScript(fileName))
      {
         updateOutput("Cannot read file " + fileName);
      }
   }
   else
   {
      updateOutput("No filename specified.");
   }
   return;
}

bool ScriptingWidget::loadScript(QString fileName)
{
   bool status = false;
   updateOutput("Loading script file " + fileName + " into editor");
   QFile file(fileName);
   if (file.open(QIODevice::ReadOnly))
   {
      QTextStream in(&file);
      in.setCodec("UTF-8");
      QString script = in.readAll();
      QTextEdit *scriptEditor = ui->scriptEditor;
      scriptEditor->append(script);
      file.close();
      setModeFromFile(fileName);
      status = true;
   }
   return status;
}

void ScriptingWidget::saveScript(QString fileName)
{
   QFile file(fileName);
   if (file.open(QIODevice::WriteOnly))
   {
      QTextStream out(&file);
      QTextEdit *scriptEditor = ui->scriptEditor;
      out << scriptEditor->document()->toPlainText();
      file.close();
   }
   else
   {
      updateOutput("Cannot write file " + fileName);
   }

}

void ScriptingWidget::saveScript()
{
   /* Gets a valid fileName from the user. Creates a file from the contents of the script editor.
   */

   QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                   "", tr("Script Files (*.js;*.m)"));

   if (!fileName.isEmpty())
   {
      updateOutput("Saving script from editor to file " + fileName);
      saveScript(fileName);
   }
   else
   {
      updateOutput("No filename specified.");
   }

   return;
}

ScriptRunner *ScriptingWidget::getScriptRunner()
{
   return scriptRunner;
}

void ScriptingWidget::scriptError()
{
   /* Prints an error in the scriptOutput.
   */

   updateOutput("Error in script execution");
}

void ScriptingWidget::scriptDone(bool success)
{
   if (success)
   {
      updateOutput("Script successful");
      processNextSection();
   }
   else
   {
      setScriptIsRunning(false);
      updateOutput("Script failed");
      //KSA replaced empty() with clear(), as empty() is equivalent to isEmpty()
      sections.clear();
   }
}

void ScriptingWidget::setScriptIsRunning(bool scriptIsRunning)
{
   this->scriptIsRunning = scriptIsRunning;
   ui->executeButton->setEnabled(!scriptIsRunning);
   ui->saveButton->setEnabled(!scriptIsRunning);
   ui->loadButton->setEnabled(!scriptIsRunning);
   ui->scriptLine->setEnabled(!scriptIsRunning);
   if (isInitScript)
   {
      isInitScript = false;
   }
}




