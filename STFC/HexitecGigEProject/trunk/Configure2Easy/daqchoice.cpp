#include "daqchoice.h"

#include <QLabel>
#include <QHBoxLayout>

DAQChoice::DAQChoice(QSettings *settings, QWidget *parent) :
   QWidget(parent)
{
   setStatusTip("Turn Data Acquisition support on/off");
   this->settings = settings;
   QLabel* label = new QLabel("Data Acquisition");
   checkBox = new QCheckBox("Data Acquisition" , this);
   if (settings->contains("DataAcquisition"))
   {
      if (settings->value("DataAcquisition").toString() == "On")
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

bool DAQChoice::on()
{
   bool on = false;

   if (checkBox->checkState() == Qt::Checked)
      on = true;

   return on;
}

void DAQChoice::updateSettings()
{
   switch (checkBox->checkState())
   {
   case Qt::Unchecked:
      settings->setValue("DataAcquisition", "Off");
      break;
   case Qt::Checked:
      settings->setValue("DataAcquisition", "On");
      break;
   }
}

void DAQChoice::removeSettings()
{
   settings->remove("DataAcquisition");
}
