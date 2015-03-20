#include "voltagesourcefactory.h"
#include "parameters.h"

VoltageSourceFactory *VoltageSourceFactory::vsfInstance = 0;

VoltageSourceFactory::VoltageSourceFactory(QWidget *parent) :
   QWidget(parent)
{
   connect(this, SIGNAL(addObject(QObject*, bool, bool)), ScriptingWidget::instance()->getScriptRunner(),
           SLOT(addObject(QObject*, bool, bool)));
}

VoltageSourceFactory *VoltageSourceFactory::instance()
{
   if (vsfInstance == 0)
   {
      vsfInstance = new VoltageSourceFactory();
   }
   return vsfInstance;
}

Keithley *VoltageSourceFactory::createKeithley(QString name)
{
   QString detectorFilename = Parameters::twoEasyIniFilename;
   QSettings settings(QSettings::UserScope, "TEDDI", "2Easy");
   if (settings.contains("2EasyIniFilename"))
   {
      detectorFilename = settings.value("2EasyIniFilename").toString();
   }

   keithley = new Keithley();
   keithley->setProperty("objectName", name);
   keithley->initialise(detectorFilename);
   emit addObject(keithley, true, true);

   return keithley;
}

Keithley *VoltageSourceFactory::getKeithley()
{
   return keithley;
}
