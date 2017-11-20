#ifndef INIFILE_H
#define INIFILE_H

#define VALID 0
#define INVALID -1

#include <QString>
#include <QStringList>
#include <QSettings>
#include <QHash>

class IniFile
{
public:
   IniFile(QString filename);
   void readIniFile();
   void writeIniFile();
   bool getBool(QString name);
   int getInt(QString name);
   float getFloat(QString name);
   double getDouble(QString name);
   QString getString(QString name);
   char *getCharArray(QString name);
   QStringList getStringList(QString name);
   QVariant setParameter(QString name, QVariant value);
private:
   QSettings *iniParameters;
   QString filename;
   QHash<QString, QVariant> parameters;
   QVariant getParameter(QString name);
};

#endif // INIFILE_H
