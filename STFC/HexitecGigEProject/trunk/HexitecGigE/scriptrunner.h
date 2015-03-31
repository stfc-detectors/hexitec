#ifndef SCRIPTRUNNER_H
#define SCRIPTRUNNER_H

//Fix MSVC 2013 compiler complaint:
#define TRUE 1
#define FALSE 0

#include <QThread>
#include <QString>
#include <QtScript>
#include <QHash>

#include "reservable.h"
#include "reservation.h"
#include "objectreserver.h"
#include "inifile.h"
#include "serialport.h"

class ScriptRunner : public QThread
{
   Q_OBJECT
private:
   QScriptEngine *engine;
   void run();
   QList<QObject *> objects;
   QList<QObject *> scriptObjects;
   QList<QObject *> guiObjects;
   QString script;
   QList<SerialPort *> serialPortList;
   SerialPort *serialPort2;
   IniFile *detectorIniFile;

public:
   ScriptRunner(QObject *parent = 0);
   void setScript(QString script);
   QList<QObject *> getScriptObjects();
   QList<QObject *> getGuiObjects();
   QList<QObject *> getObjects();
   QString listObjects();

   static QScriptValue printReplacement(QScriptContext *qsc, QScriptEngine *engine);
   static QScriptValue list(QScriptContext *qsc, QScriptEngine *engine);
   static QScriptValue find(QScriptContext *qsc, QScriptEngine *engine);
   void configueSerialPorts(QString twoEasyFilename);
signals:
   void updateOutput(QString message);
   void scriptDone(bool status);

public slots:
   void addObject(QObject *object, bool scripting, bool gui);
};

#endif // SCRIPTRUNNER_H
