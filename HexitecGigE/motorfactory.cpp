#include "motorfactory.h"

MotorFactory *MotorFactory::mfInstance = 0;

MotorFactory::MotorFactory(QWidget *parent) :
   QWidget(parent)
{
   motorModel = MotorModel::getMotorModelHandle();
   connect(this, SIGNAL(addObject(QObject*, bool, bool)), ScriptingWidget::instance()->getScriptRunner(),
           SLOT(addObject(QObject*, bool, bool)));
}

MotorFactory *MotorFactory::instance()
{
   if (mfInstance == 0)
   {
      mfInstance = new MotorFactory();
   }
   return mfInstance;
}

MotorFactory::~MotorFactory()
{
   delete motorModel;
}

Motor *MotorFactory::createMotor(QString name, MotorType type, int id)
{
/* Declare motors.
   The motors are hardcoded at the moment, they should
   eventually be configurable via an initialisation process.
*/
   switch (type)
   {
   case DUMMY:
      motor = new DummyMotor(name);
      break;
   case GALIL:
      motor = new GalilMotor(name, id);
      break;
   case NEWPORT:
      motor = new NewportXpsMotor(name, id);
      break;
   }
   motor->setProperty("objectName", name);
   motor->setSelected(false);
   motorModel->addMotor(motor);
   emit addObject(motor);

   return motor;
}
