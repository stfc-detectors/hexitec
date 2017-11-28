#include "voltagesourcefactory.h"
#include "parameters.h"

VoltageSourceFactory *VoltageSourceFactory::vsfInstance = 0;

VoltageSourceFactory::VoltageSourceFactory(QWidget *parent) :
   QWidget(parent)
{
//   connect(this, SIGNAL(addObject(QObject*, bool, bool)), ScriptingWidget::instance()->getScriptRunner(),
//           SLOT(addObject(QObject*, bool, bool)));
}

VoltageSourceFactory *VoltageSourceFactory::instance()
{
   if (vsfInstance == 0)
   {
      vsfInstance = new VoltageSourceFactory();
   }
   return vsfInstance;
}

HV *VoltageSourceFactory::createHV(QString name)
{
   QString detectorFilename = Parameters::twoEasyIniFilename;
   QSettings settings(QSettings::UserScope, "TEDDI", "HexitecGigE");
   if (settings.contains("hexitecGigEIniFilename"))
   {
      detectorFilename = settings.value("hexitecGigEIniFilename").toString();
   }

   hv = new HV();
   hv->setProperty("objectName", name);
   hv->initialise(detectorFilename);
   emit addObject(hv, true, true);

   return hv;
}

HV *VoltageSourceFactory::getHV()
{
   return hv;
}
