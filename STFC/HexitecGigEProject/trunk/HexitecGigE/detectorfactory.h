#ifndef DETECTORFACTORY_H
#define DETECTORFACTORY_H

/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX

#include "aspectdetector.h"
#include "scriptingwidget.h"
#include "inifile.h"
#include "filewriter.h"
#include "detectormonitor.h"

class DetectorFactory : public QWidget
{
   Q_OBJECT
public:
   ~DetectorFactory();
   static DetectorFactory *instance();
   AspectDetector *createAspectDetector(QString name);
   AspectDetector *getAspectDetector();
   DetectorMonitor *getDetectorMonitor();
   FileWriter *getImageIndicatorFile();
protected:
   explicit DetectorFactory(QWidget *parent = 0);
private:
   AspectDetector *aspectDetector;
   static DetectorFactory *dfInstance; 
   IniFile *detectorIniFile;
   FileWriter *imageIndicatorFile;
   DetectorMonitor *detectorMonitor;
signals:
    void addObject(QObject *object, bool scripting = TRUE, bool gui = TRUE);
};

#endif // DETECTORFACTORY_H
