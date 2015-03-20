#include "detectorfactory.h"
#include "parameters.h"

DetectorFactory *DetectorFactory::dfInstance = 0;

DetectorFactory::DetectorFactory(QWidget *parent) :
   QWidget(parent)
{
   connect(this, SIGNAL(addObject(QObject*, bool, bool)), ScriptingWidget::instance()->getScriptRunner(),
           SLOT(addObject(QObject*, bool, bool)));
}

DetectorFactory *DetectorFactory::instance()
{
   if (dfInstance == 0)
   {
      dfInstance = new DetectorFactory();
   }
   return dfInstance;
}

AspectDetector *DetectorFactory::createAspectDetector(QString name)
{
   QString imageIndicatorFilename;
   QString aspectFilename = Parameters::aspectIniFilename;
   QString twoEasyFilename = Parameters::twoEasyIniFilename;

   QSettings settings(QSettings::UserScope, "TEDDI", "2Easy");
   if (settings.contains("aspectIniFilename"))
   {
      aspectFilename = settings.value("aspectIniFilename").toString();
   }
   if (settings.contains("2EasyIniFilename"))
   {
      twoEasyFilename = settings.value("2EasyIniFilename").toString();
   }

   detectorIniFile = new IniFile(twoEasyFilename);

   imageIndicatorFilename = twoEasyFilename;
   imageIndicatorFilename.truncate(imageIndicatorFilename.lastIndexOf("/"));
   imageIndicatorFile = new FileWriter(imageIndicatorFilename, detectorIniFile->getString("Detector/Image Indicator File"));

   aspectDetector = new AspectDetector(aspectFilename, twoEasyFilename);
   aspectDetector->setProperty("objectName", name);
   emit addObject(aspectDetector);

   QByteArray tcPort = detectorIniFile->getString("Detector Monitor/Temperature Controller Port").toUtf8();
   int loggingInterval = detectorIniFile->getInt("Detector Monitor/Logging Interval");
   detectorMonitor = new DetectorMonitor(aspectDetector, tcPort.data(), loggingInterval);

   return aspectDetector;
}

AspectDetector *DetectorFactory::getAspectDetector()
{
   return aspectDetector;
}

DetectorMonitor *DetectorFactory::getDetectorMonitor()
{
   return detectorMonitor;
}

FileWriter *DetectorFactory::getImageIndicatorFile()
{
   return imageIndicatorFile;
}

DetectorFactory::~DetectorFactory()
{
   dfInstance = 0;
   detectorMonitor->stop();
}
