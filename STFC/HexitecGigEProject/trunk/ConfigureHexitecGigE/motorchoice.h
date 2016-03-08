#ifndef MOTORCHOICE_H
#define MOTORCHOICE_H

#include <QWidget>
#include <QString>
#include <QButtonGroup>
#include <QSpinBox>
#include <QSettings>

class MotorChoice : public QWidget
{
   Q_OBJECT

public:
   static const int GALIL = 1;
   static const int DUMMY = 2;
   static const int NEWPORT = 3;
   MotorChoice(QString motor, QSettings *settings, QWidget *parent = 0);
   int motorType();
   QString getMotor();
   void updateSettings();
   void removeSettings();

private:
   QString motor;
   QButtonGroup* group;
   QSettings *settings;
   QSpinBox *galilId;
   QSpinBox *newportId;
   QSpinBox *gearing;

private slots:
   void galilToggled(bool checked);
   void newportToggled(bool checked);
};

#endif // MOTORCHOICE_H
