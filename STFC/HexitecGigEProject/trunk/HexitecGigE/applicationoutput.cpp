#include "applicationoutput.h"

ApplicationOutput *ApplicationOutput::applicationOutputInstance = 0;

ApplicationOutput::ApplicationOutput(QWidget *parent)
   : QTextBrowser(parent)
{
}

ApplicationOutput *ApplicationOutput::instance(QWidget *parent)
{
   if (applicationOutputInstance == 0)
   {
      applicationOutputInstance = new ApplicationOutput(parent);
   }
   return applicationOutputInstance;
}

void ApplicationOutput::writeMessage(QString message)
{
   setTextColor(QColor(Qt::black));
   append(message);
}

void ApplicationOutput::writeWarning(QString message)
{
   setTextColor(QColor(Qt::darkGreen));
   append("Warning: " + message);
   setTextColor(QColor(Qt::black));
}

void ApplicationOutput::writeError(QString message)
{
   setTextColor(QColor(Qt::red));
   append("Error: " + message);
   setTextColor(QColor(Qt::black));
}

void ApplicationOutput::append(QString message)
{
   if (message.endsWith('\n'))
   {
      message.chop(1);
   }
   QTextEdit::append(message);
}
