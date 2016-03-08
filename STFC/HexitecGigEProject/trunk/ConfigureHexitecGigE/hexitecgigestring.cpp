#include "hexitecgigestring.h"
#include <QHBoxLayout>

HexitecGigEString::HexitecGigEString(QString name, QSettings *settings, QWidget *parent) :
   QWidget(parent)
{
   setStatusTip("Alter a named string");
   this->name = name;
   this->settings = settings;
   QLabel* label = new QLabel(name);
   lineEdit = new QLineEdit(this);
   if (settings->contains(name))
   {
      lineEdit->setText(settings->value(name).toString());
   }
   else
   {
      qDebug()<<"NO VALUE for"<<name;
   }

   QHBoxLayout* layout = new QHBoxLayout(this);
   layout->addWidget(label);
   layout->addWidget(lineEdit);
}

void HexitecGigEString::updateSettings()
{
   settings->setValue(name, lineEdit->text());
}

void HexitecGigEString::removeSettings()
{
   settings->remove(name);
}
QString HexitecGigEString::getText()
{
   return lineEdit->text();
}
