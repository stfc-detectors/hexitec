#ifndef MATLABCHOICE_H
#define MATLABCHOICE_H

#include <QWidget>
#include <QCheckBox>
#include <QSettings>

class MatlabChoice : public QWidget
{
public:
   explicit MatlabChoice(QSettings *settings, QWidget *parent = 0);
   bool on();
   void updateSettings();
   void removeSettings();

private:
   QCheckBox *checkBox;
   QSettings *settings;
};

#endif // MATLABCHOICE_H
