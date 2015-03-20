#include "temperature.h"
#include <QDebug>

Temperature::Temperature(QObject *parent)
{
   connect(this, SIGNAL(writeError(QString)), parent, SLOT(handleWriteError(QString)));
   connect(this, SIGNAL(writeMessage(QString)), parent, SLOT(handleWriteMessage(QString)));
}

void Temperature::handleWriteError(QString message)
{
   emit writeError(message);
}

void Temperature::handleWriteMessage(QString message)
{
   emit writeMessage(message);
}

void Temperature::handleDestruction()
{
   destruction();
}
