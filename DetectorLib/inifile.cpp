#include "inifile.h"

IniFile::IniFile(QString filename)
{
   this->filename = filename;
   iniParameters = new QSettings(filename, QSettings::IniFormat);
   readIniFile();
}

void IniFile::readIniFile()
{
   QStringList keys = iniParameters->allKeys();

   foreach (QString key, keys)
   {
      parameters[key] = iniParameters->value(key);
   }
}

void IniFile::writeIniFile()
{
   QStringList keys = parameters.keys();

   foreach (QString key, keys)
   {
      iniParameters->setValue(key, parameters[key]);
   }
}

QVariant IniFile::getParameter(QString name)
{
   QVariant parameter;

   if (parameters.contains(name))
   {
      parameter = parameters[name];
   }
   else
   {
      parameter = QVariant(INVALID);
   }

   return parameter;
}

bool IniFile::getBool(QString name)
{
   return getParameter(name).toBool();
}

int IniFile::getInt(QString name)
{
   return getParameter(name).toInt();
}

float IniFile::getFloat(QString name)
{
   return getParameter(name).toFloat();
}

double IniFile::getDouble(QString name)
{
   return getParameter(name).toDouble();
}

QString IniFile::getString(QString name)
{
   return getParameter(name).toString();
}

char *IniFile::getCharArray(QString name)
{
   QByteArray ba = getParameter(name).toByteArray();
   return ba.data();
}

QStringList IniFile::getStringList(QString name)
{
  return getParameter(name).toStringList();
}

QVariant IniFile::setParameter(QString name, QVariant value)
{
   QVariant status;

   if (parameters.contains(name))
   {
      parameters[name] = value;
   }
   else
   {
      parameters.insert(name, value);
   }
   status = QVariant(VALID);

   return status;
}

