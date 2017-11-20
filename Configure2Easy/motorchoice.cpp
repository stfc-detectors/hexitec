#include "motorchoice.h"
#include <QLabel>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QButtonGroup>

MotorChoice::MotorChoice(QString motor, QSettings *settings, QWidget *parent) :
   QWidget(parent)
{
   this->motor = motor;
   this->settings = settings;
   setStatusTip("Set motor type and parameters for : " + motor);

   QLabel *label = new QLabel(motor);
   QRadioButton *dummyButton = new QRadioButton("Dummy" , this);
   QRadioButton *galilButton = new QRadioButton("Galil" , this);
   QRadioButton *newportButton = new QRadioButton("Newport" , this);
   QLabel *galilLabel = new QLabel("Galil ID");
   galilId = new QSpinBox();
   QLabel *newportLabel = new QLabel("Newport ID");
   newportId = new QSpinBox();
   QLabel *gearingLabel = new QLabel("Gearing");
   gearing = new QSpinBox();
   QHBoxLayout *layout = new QHBoxLayout(this);

   connect(galilButton, SIGNAL(toggled(bool)), this, SLOT(galilToggled(bool)));
   galilId->setMinimum(1);
   galilId->setMaximum(8);

   connect(newportButton, SIGNAL(toggled(bool)), this, SLOT(newportToggled(bool)));
   newportId->setMinimum(1);
   newportId->setMaximum(8);

   gearing->setMinimum(1);
   gearing->setMaximum(999999);

   layout->addWidget(label);
   layout->addWidget(dummyButton);
   layout->addWidget(newportButton);
   layout->addWidget(newportLabel);
   layout->addWidget(newportId);
   layout->addWidget(galilButton);
   layout->addWidget(galilLabel);
   layout->addWidget(galilId);
   layout->addWidget(gearingLabel);
   layout->addWidget(gearing);

   /*Create QButonGroup and add Buttons.
      Adding Buttons into QButtonGroup does not change anything visually.
      QButtonGroup is just a logical grouping. */
   group = new QButtonGroup(this);
   group->addButton(dummyButton);
   group->addButton(newportButton);
   group->addButton(galilButton);
   group->setId(dummyButton, MotorChoice::DUMMY);
   group->setId(galilButton, MotorChoice::GALIL);
   group->setId(newportButton, MotorChoice::NEWPORT);

   if (settings->contains(motor))
   {
      galilToggled(false);
      newportToggled(false);
      if (settings->value(motor) == MotorChoice::DUMMY)
      {
         dummyButton->setChecked(true);
      }
      else if (settings->value(motor) == MotorChoice::GALIL)
      {
         galilButton->setChecked(true);
         galilId->setValue(settings->value(motor+"Id").toInt());
         galilToggled(true);
      }
      else if (settings->value(motor) == MotorChoice::NEWPORT)
      {
         newportButton->setChecked(true);
         newportId->setValue(settings->value(motor+"Id").toInt());
         newportToggled(true);
      }
      gearing->setValue(settings->value(motor+"Gearing").toInt());
   }

   group->setExclusive(true);
}

QString MotorChoice::getMotor()
{
   return motor;
}

int MotorChoice::motorType()
{
   int type = 0;

   type = group->checkedId();

   return type;
}

void MotorChoice::updateSettings()
{
   switch (motorType())
   {
   case MotorChoice::GALIL:
      settings->setValue(motor, MotorChoice::GALIL);
      settings->setValue(motor+"Id", galilId->value());
      break;
   case MotorChoice::NEWPORT:
      settings->setValue(motor, MotorChoice::NEWPORT);
      settings->setValue(motor+"Id", newportId->value());
      break;
   case MotorChoice::DUMMY:
      settings->setValue(motor, MotorChoice::DUMMY);
      break;
   }
   settings->setValue(motor+"Gearing", gearing->value());
}

void MotorChoice::removeSettings()
{
   settings->remove(motor);
   settings->remove(motor+"Id");
   settings->remove(motor+"Gearing");
}

void MotorChoice::galilToggled(bool checked)
{
   galilId->setDisabled(!checked);
}

void MotorChoice::newportToggled(bool checked)
{
   newportId->setDisabled(!checked);
}
