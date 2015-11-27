#include <QFileInfo>

#include "scriptrunner.h"
#include "scriptingwidget.h"
#include "parameters.h"

ScriptRunner::ScriptRunner(QObject *parent) :
   QThread(parent)
{
   engine = new QScriptEngine();
   // This replaces the engine's print method with the static method printReplacement
   engine->globalObject().setProperty("print", engine->newFunction(&printReplacement));
   // This adds the static list function which lists all the objects known to the engine.
   engine->globalObject().setProperty("list", engine->newFunction(&list));

   QString twoEasyFilename = Parameters::twoEasyIniFilename;

   QSettings settings(QSettings::UserScope, "TEDDI", "HexitecGigE");
   if (settings.contains("hexitecGigEIniFilename"))
   {
      twoEasyFilename = settings.value("hexitecGigEIniFilename").toString();
   }

   if (QFileInfo(twoEasyFilename).exists())
   {
       configueSerialPorts(twoEasyFilename);
   }

}

void ScriptRunner::configueSerialPorts(QString twoEasyFilename)
{
    IniFile *scriptingIniFile = new IniFile(twoEasyFilename);
    QStringList names = scriptingIniFile->getStringList("Serial_Ports/Name");
    QStringList ports = scriptingIniFile->getStringList("Serial_Ports/Port");
    QStringList terminators = scriptingIniFile->getStringList("Serial_Ports/Write_Terminator");
    QStringList baudRates = scriptingIniFile->getStringList("Serial_Ports/Baud_Rate");
    QStringList dataBits = scriptingIniFile->getStringList("Serial_Ports/Data_Bits");
    QStringList stopBits = scriptingIniFile->getStringList("Serial_Ports/Stop_Bits");
    QStringList parities = scriptingIniFile->getStringList("Serial_Ports/Parity");

    SerialPort *newSerialPort;
    for (int i = 0; i < names.size(); ++i)
    {
        if (i < ports.length())
        {
            newSerialPort = new SerialPort(ports.at(i));
        }
        if (i < baudRates.length())
        {
            newSerialPort->setBaudRate(baudRates.at(i));
        }
        if (i < dataBits.length())
        {
            newSerialPort->setDataBits(dataBits.at(i));
        }
        if (i < stopBits.length())
        {
            newSerialPort->setStopBits(stopBits.at(i));
        }
        if (i < parities.length())
        {
            newSerialPort->setParity(parities.at(i));
        }
        if (newSerialPort->open(QIODevice::ReadWrite))
        {
            serialPortList.append(newSerialPort);
            serialPortList.at(i)->setProperty("objectName", names.at(i));
            addObject(newSerialPort, FALSE, FALSE);
            newSerialPort->setWriteTermination(terminators.at(i));
        }
    }
}

void ScriptRunner::setScript(QString script)
{
   this->script = script;
}

void ScriptRunner::addObject(QObject *object, bool scripting, bool gui)
{

   engine->globalObject().setProperty(object->objectName(), engine->newQObject(object));
   if (scripting)
   {
      scriptObjects.append(object);
   }
   if (gui)
   {
      guiObjects.append(object);
   }
   objects.append(object);
}

QList<QObject *> ScriptRunner::getScriptObjects()
{
   return scriptObjects;
}

QList<QObject *> ScriptRunner::getGuiObjects()
{
   return guiObjects;
}

QList<QObject *> ScriptRunner::getObjects()
{
   return objects;
}



QString ScriptRunner::listObjects()
{
   QString list = QString("");
   QListIterator<QObject *> it = QListIterator<QObject *>(objects);
   while (it.hasNext())
   {
      list += (it.next()->objectName() + "\n");

   }
   return list;
}

void ScriptRunner::run()
{
   emit scriptDone(!engine->evaluate(script).isError());
}

/*
  This replaces the script print method (see constructor). The first argument will be what needs printing.
  We use the fact that ScriptingWidget is a singleton to get an instance of ScriptRunner and emit an updateOutput.
  This is better than calling ScriptingWidget's updateOutput directly because printReplacement will be called from
  a different thread.
  */
QScriptValue ScriptRunner::printReplacement(QScriptContext *qsc, QScriptEngine *engine)
{
   QString toPrint = qsc->argument(0).toString();
   emit ScriptingWidget::instance()->getScriptRunner()->updateOutput(toPrint);
   return QScriptValue();
}

/*
  This allows scripts to list all the (ScriptRunner) objects known to the ScriptRunner or if there is an
  argument ALL the objects known to the engine.
  */
QScriptValue ScriptRunner::list(QScriptContext *qsc, QScriptEngine *engine)
{
   ScriptRunner *instance = ScriptingWidget::instance()->getScriptRunner();
   QString theList = instance->listObjects();
   if (qsc->argumentCount() == 1)
   {
      theList = "";
      QScriptValueIterator it(engine->globalObject());
      while (it.hasNext())
      {
         it.next();
         theList += (it.name() + "\n");
      }
   }
   emit instance->updateOutput(theList);
   return QScriptValue();
}

