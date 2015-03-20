#ifndef TWOEASYSTRING_H
#define TWOEASYSTRING_H

#include <QWidget>
#include <QLineEdit>
#include <QSettings>
#include <QLabel>
#include <QDebug>
#include <QString>

class TwoEasyString : public QWidget
{
public:
   explicit TwoEasyString(QString name, QSettings *settings, QWidget *parent = 0);
   bool on();
   void updateSettings();
   void removeSettings();
   QString getText();

private:
   QString name;
   QLineEdit *lineEdit;
   QSettings *settings;
};

#endif // TWOEASYSTRING_H
