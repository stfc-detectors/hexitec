#ifndef HEXITECGIGESTRING_H
#define HEXITECGIGESTRING_H

#include <QWidget>
#include <QLineEdit>
#include <QSettings>
#include <QLabel>
#include <QDebug>
#include <QString>

class HexitecGigEString : public QWidget
{
public:
   explicit HexitecGigEString(QString name, QSettings *settings, QWidget *parent = 0);
   bool on();
   void updateSettings();
   void removeSettings();
   QString getText();

private:
   QString name;
   QLineEdit *lineEdit;
   QSettings *settings;
};

#endif // HEXITECGIGESTRING_H
