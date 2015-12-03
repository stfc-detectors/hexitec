#ifndef MATLAB_H
#define MATLAB_H

#include <QObject>
#include <QSettings>
#include <QThread>
#include "applicationoutput.h"
#include "slice.h"
#include <engine.h>

class matlab : public QThread
{
   Q_OBJECT

public:
   enum {SUCCESS, CREATE_ERR, RETRIEVE_ERR, DIMENSION_ERR};
   static matlab *instance();
   int sendArrayToMatlab(int *dimensions, double *data, QString varName);
   int getSliceFromMatlab(Slice *slice, QString);
   int sendVectorToMatlab(QVector <double> &, QString);
   int getVectorFromMatlab(QVector <double> &, QString);

   double *getArray(QString varName);
   QString getBuffer();
   void setScript(QString script);
   void startScript();

   QStringList listVariables();
private:
   matlab(QObject *parent = 0);
   ~matlab();
   static matlab *matlabInstance;
   void run();
   bool activeMatlab;

   Engine* engine;
   static const int BUFFER_SIZE = 10000;
   char buffer[BUFFER_SIZE + 1];
   bool open(const char *startcmd = NULL);
   bool close();
   bool setOutputBuffer(Engine *engine);
   bool setVisible(bool visibility = true);
   QString script;
   bool startScriptFlag;
   bool mainThreadFlag;
   double *createTestMask();

public slots:
   bool evalString(QString);

private slots:
   void checkConnection();

signals:
   void checkRequired();
   void writeMessage(QString message);
   void writeWarning(QString message);
   void writeError(QString message);
   void scriptDone(bool status);
   void updateOutput(QString message);
   void matlabStatus(bool status);
};

#endif // MATLAB_H
