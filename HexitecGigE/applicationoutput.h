#ifndef APPLICATIONOUTPUT_H
#define APPLICATIONOUTPUT_H

//#include <QtGui>
#include <QTextBrowser>


class ApplicationOutput : public QTextBrowser
{
   Q_OBJECT
public:
   static ApplicationOutput *instance(QWidget *parent = 0);

private:
   ApplicationOutput(QWidget *parent = 0);
   static ApplicationOutput *applicationOutputInstance;
   void append(QString message);

public slots:
   void writeMessage(QString message);
   void writeWarning(QString message);
   void writeError(QString message);
};

#endif // APPLICATIONOUTPUT_H
