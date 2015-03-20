#ifndef VOLTAGESOURCEFACTORY_H
#define VOLTAGESOURCEFACTORY_H

/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX

#include "keithley.h"
#include "scriptingwidget.h"

class VoltageSourceFactory : public QWidget
{
   Q_OBJECT
public:
   static VoltageSourceFactory *instance();
   Keithley *createKeithley(QString name);
   Keithley *getKeithley();
protected:
   explicit VoltageSourceFactory(QWidget *parent = 0);
private:
   Keithley *keithley;
   static VoltageSourceFactory *vsfInstance; 
signals:
    void addObject(QObject *object, bool, bool);
};

#endif // VOLTAGESOURCEFACTORY_H
