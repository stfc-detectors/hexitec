#ifndef DAQCHOICE_H
#define DAQCHOICE_H

#include <QWidget>
#include <QCheckBox>
#include <QSettings>

class DAQChoice : public QWidget
{
public:
   explicit DAQChoice(QSettings *settings, QWidget *parent = 0);
   bool on();
   void updateSettings();
   void removeSettings();

private:
   QCheckBox *checkBox;
   QSettings *settings;
};

#endif // DAQCHOICE_H
