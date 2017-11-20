#ifndef MOTORFACTORY_H
#define MOTORFACTORY_H

#include "motormodel.h"
#include "dummymotor.h"
#include "galilmotor.h"
#include "newportxpsmotor.h"
#include "scriptingwidget.h"

enum MotorType {DUMMY, GALIL, NEWPORT};

class MotorFactory : public QWidget
{
   Q_OBJECT
public:
   static MotorFactory *instance();
   ~MotorFactory();
   Motor *createMotor(QString name, MotorType type, int id = 0);
protected:
   explicit MotorFactory(QWidget *parent = 0);
private:
   Motor *motor;
   static MotorFactory *mfInstance; 
   MotorModel *motorModel;
signals:
   void addObject(QObject *object, bool scripting = TRUE, bool gui = TRUE);
};

#endif // MOTORFACTORY_H
