#ifndef VOLTAGESOURCEFACTORY_H
#define VOLTAGESOURCEFACTORY_H

/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX

#include <QWidget>
#include "hv.h"
//#include "scriptingwidget.h"

class VoltageSourceFactory : public QWidget
{
   Q_OBJECT
public:
   static VoltageSourceFactory *instance();
       HV *createHV(QString name);
       HV *getHV();
protected:
   explicit VoltageSourceFactory(QWidget *parent = 0);
private:
   HV *hv;
   static VoltageSourceFactory *vsfInstance; 
signals:
    void addObject(QObject *object, bool, bool);
};

#endif // VOLTAGESOURCEFACTORY_H
