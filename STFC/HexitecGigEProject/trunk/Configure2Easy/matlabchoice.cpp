#include "matlabchoice.h"
#include <QLabel>
#include <QHBoxLayout>

MatlabChoice::MatlabChoice(QSettings *settings, QWidget *parent) :
   QWidget(parent)
{
   setStatusTip("Turn MATLAB support on/off");
   this->settings = settings;
   QLabel* label = new QLabel("MATLAB");
   checkBox = new QCheckBox("Matlab" , this);
   if (settings->contains("MATLAB"))
   {
      if (settings->value("MATLAB").toString() == "On")
      {
         checkBox->setCheckState(Qt::Checked);
      }
      else
      {
         checkBox->setCheckState(Qt::Unchecked);
      }
   }

   QHBoxLayout* layout = new QHBoxLayout(this);
   layout->addWidget(label);
   layout->addWidget(checkBox);
}

bool MatlabChoice::on()
{
   bool on = false;

   if (checkBox->checkState() == Qt::Checked)
      on = true;

   return on;
}

void MatlabChoice::updateSettings()
{
   switch (checkBox->checkState())
   {
   case Qt::Unchecked:
      settings->setValue("MATLAB", "Off");
      break;
   case Qt::Checked:
      settings->setValue("MATLAB", "On");
      break;
   }
}

void MatlabChoice::removeSettings()
{
   settings->remove("MATLAB");
}
