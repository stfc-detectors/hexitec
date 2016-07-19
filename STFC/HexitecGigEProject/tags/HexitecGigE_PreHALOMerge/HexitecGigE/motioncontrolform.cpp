#include "motioncontrolform.h"
#include "ui_motioncontrolform.h"
#include "scriptingwidget.h"
#include "dummymotor.h"

MotionControlForm::MotionControlForm(QWidget *parent) :
   QWidget(parent),
   ui(new Ui::MotionControlForm)
{
   /* The DataAcquisitionFactory constructs motors via the MotorFactory (and if the future will create
      other objects required for data aquisition. It must be called or no motors will be created. The
      DataAcquisitionFactory is a singleton. The first call to it creates it, subsequent calls to it
      return the same DataAcquisitionFactory
   */
   motorModel = MotorModel::getMotorModelHandle();
   ui->setupUi(this);
   setLayout(ui->gridLayout);
   ui->gridLayout->setContentsMargins(5, 5, 5, 5);
   motors = motorModel->getMotors();
   initialiseForm();

   mainWindow = new QMainWindow();
   mainWindow->setCentralWidget(this);

   connect(ui->initialiseButton, SIGNAL(pressed()), this, SLOT(initialisePressed()));
   connect(ui->driveButton, SIGNAL(pressed()), this, SLOT(drivePressed()));
   connect(ui->stopButton, SIGNAL(pressed()), this, SLOT(stopPressed()));  
}

MotionControlForm::~MotionControlForm()
{
   delete ui;
}

QMainWindow *MotionControlForm::getMainWindow()
{
   return mainWindow;
}

void MotionControlForm::initialiseForm()
{
   QTableView *motorTableView = ui->motorTableView;
   QModelIndex qmi;
   QList<QString> headers;

   headers << "Name" <<"Status" << "Position" << "Move" << "Mode" << "Start";

   connect(this, SIGNAL(addObject(QObject*, bool, bool)), ScriptingWidget::instance()->getScriptRunner(),
           SLOT(addObject(QObject*, bool, bool)));
   motorModel->setColumnHeaderData(headers);
   motorTableView->setModel(motorModel);
   motorTableView->setItemDelegate(new MotorDelegate(headers.indexOf("Mode"), headers.indexOf("Start")));

   int count = motors.count();
   for (int i = 0; i < count; i++)
   {
      qmi = motorModel->index(i, headers.indexOf("Mode"));
      motorTableView->openPersistentEditor(qmi);
      qmi = motorModel->index(i, headers.indexOf("Start"));
      motorTableView->openPersistentEditor(qmi);
   }

   motorTableView->horizontalHeader()->setStretchLastSection(true);
}

QList<QObject *> MotionControlForm::requestedMotorsList()
{
   Motor *motor;
   QList <QObject *> requestedMotorsList;
   QListIterator<Motor*> i(motors);

   while (i.hasNext())
   {
      motor = i.next();
      if (motor->isSelected())
      {
         requestedMotorsList.append(motor);
      }
   }

   return requestedMotorsList;
}

void MotionControlForm::initialisePressed()
{
   Motor *motor;
   bool ready = true;

   QList <QObject *> rml = requestedMotorsList();
   Reservation reservation = ObjectReserver::instance()->reserveForGUI(rml);

   if (reservation.getReserved().isEmpty())
   {
      updateOutput("Could not reserve all objects, message = " + reservation.getMessage());
   }
   else
   {
      for (int i = 0; i < rml.size(); ++i)
      {
         motor = dynamic_cast <Motor *> (rml.at(i));
         if (!motor->willInitialise(true))
         {
            ready = false;
            break;
         }
      }

      if (ready)
      {
         for (int i = 0; i < rml.size(); ++i)
         {
            motor = dynamic_cast <Motor *> (rml.at(i));
            motor->initialise();
         }
      }
      else
      {
         ObjectReserver::instance()->release(reservation.getReserved(), "GUIReserver");
      }
   }
}

void MotionControlForm::drivePressed()
{
   Motor *motor;
   bool ready = true;

   QList <QObject *> rml = requestedMotorsList();
   Reservation reservation = ObjectReserver::instance()->reserveForGUI(rml);

   if (reservation.getReserved().isEmpty())
   {
      updateOutput("Could not reserve all objects, message = " + reservation.getMessage());
   }
   else
   {
      for (int i = 0; i < rml.size(); ++i)
      {
         motor = dynamic_cast <Motor *> (rml.at(i));
         if (!motor->willGo(true))
         {
            ready = false;
            break;
         }
      }

      if (ready)
      {
         for (int i = 0; i < rml.size(); ++i)
         {
            motor = dynamic_cast <Motor *> (rml.at(i));
            motor->begin();
         }
      }
      else
      {
         ObjectReserver::instance()->release(reservation.getReserved(), "GUIReserver");
      }
   }
}

void MotionControlForm::stopPressed()
{
   Motor *motor;
   QListIterator<Motor*> i(motors);
   QString name;

   while (i.hasNext())
   {
      motor = i.next();
      if (motor->isSelected())
      {
         name = motor->property("objectName").toString();
         motor->stop();
         ui->daqOutput->append("Stopping motor " + name);
      }
   }
}

void MotionControlForm::updateOutput(QString message)
{
   ui->daqOutput->append(message);
}
