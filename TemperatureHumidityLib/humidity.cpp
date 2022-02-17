#include "humidity.h"
#include <QDebug>

Humidity::Humidity(QObject *parent)
{
   connect(this, SIGNAL(writeError(QString)), parent, SLOT(handleWriteError(QString)));
}

void Humidity::handleWriteError(QString message)
{
   emit writeError(message);
}

void Humidity::handleWriteMessage(QString message)
{
   emit writeError(message);
}

void Humidity::handleDestruction()
{
   destruction();
}
