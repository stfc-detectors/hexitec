#include "objectreserver.h"

ObjectReserver *ObjectReserver::objectReserverInstance = 0;

ObjectReserver *ObjectReserver::instance()
{
   if (objectReserverInstance == 0)
   {
      objectReserverInstance = new ObjectReserver();
   }

   return objectReserverInstance;
}

ObjectReserver::ObjectReserver(QObject *parent) : QObject(parent)
{
}

ObjectReserver::~ObjectReserver()
{
}

Reservation ObjectReserver::reserveForScript(QString script,
                                             QList<QObject *> objects, QList<QObject *> freeObjects)
{
   QList<Reservable *> reserved;
   Reservable *reservable;
   QString message = "Success";
   QMutexLocker locker(&mutex);
   bool allObjectsAvailable = true;

   controlledByGuiList.clear();

   for (int i = 0; i < freeObjects.size(); ++i)
   {
      QObject *freeObject = freeObjects.at(i);
      if (objectInScript(freeObject->objectName(), script))
      {
         reservable = dynamic_cast <Reservable *> (freeObject);
         if (!isFreeForScript(freeObject))
         {
            allObjectsAvailable = false;
            break;
         }
         emit controlledByGui();
         reserved.clear();
         controlledByGuiList.append(reservable);
         //qDebug() << freeObject->objectName() <<"added to controlByGui list.";
      }
   }

   if (allObjectsAvailable)
   {
      for (int i = 0; i < objects.size(); ++i)
      {
         QString s = QString(objects.at(i)->objectName());
         if (objectInScript(objects.at(i)->objectName(), script))
         {
            //qDebug() << s << "is in script";
            reservable = dynamic_cast <Reservable *> (objects.at(i));
            if (reservable == NULL)
            {
               message = "Object cast returned NULL";
               //KSA replaced empty() with clear(), as empty() is equivalent to isEmpty()
               reserved.clear();
               break;
            }
            else if (reservable->reserve("ScriptReserver"))
            {
               emit scriptReserve(s);
               //qDebug() << "reserving for script:" << s;
               reserved.append(reservable);
            }
            else if (reservable->reservedFor().compare("ScriptReserver") == 0)
            {
               //qDebug() << "already reserved for script:" << s;
               // Deliberately do nothing - the object is already reserved but it is
               // reserved for ScriptReserver
            }
            else
            {
               message = "Object reserved by " + reservable->reservedFor();
               //qDebug() << "ObjectReserver::reserveForScript releasing (3)" << s;
               release(reserved, "ScriptReserver");
               //KSA replaced empty() with clear(), as empty() is equivalent to isEmpty()
               reserved.clear();
               break;
            }

         }
      }
   }
   else
   {
      message = "Object reserved by " + reservable->reservedFor();
      //qDebug() << "ObjectReserver::reserveForScript releasing (4)";
      release(reserved, "ScriptReserver");
      //KSA replaced empty() with clear(), as empty() is equivalent to isEmpty()
      reserved.clear();
   }

   return Reservation(reserved, message);
}

bool ObjectReserver::isFreeForScript(QObject * object)
{
   Reservable *reservable;
   bool objectFree = true;

   reservable = dynamic_cast <Reservable *> (object);

   if (reservable->reserve("ScriptReserver"))
   {
      //qDebug() << "Can be reserved for scripting so nothing else has it therefore release it and return true.";
      //qDebug() << "ObjectReserver::reserveForScript releasing (5)";
      reserved.append(reservable);
      release(reserved, "ScriptReserver");
      objectFree = true;
   }
   else
   {
      //qDebug() << "Something else has reserved the object so return false for:" << object->objectName();
      objectFree = false;
   }
   reserved.clear();

   return objectFree;
}

Reservation ObjectReserver::reserveForGUI(QList<QObject *> objects)
{
   QList<Reservable *> reserved;
   Reservable *reservable;
   QString message = "Success";
   QMutexLocker locker(&mutex);

   for (int i = 0; i < objects.size(); ++i)
   {
      QString s = QString(objects.at(i)->objectName());

      reservable = dynamic_cast <Reservable *> (objects.at(i));
      if (reservable == NULL)
      {
         message = "Object reserved by " + reservable->reservedFor();
         //qDebug() << "ObjectReserver::reserveForGUI releasing (1)" << s;
         release(reserved, "GUIReserver");
         //KSA replaced empty() with clear(), as empty() is equivalent to isEmpty()
         reserved.clear();
         break;
      }
      else if (reservable->reservedFor() == "GUIReserver")
      {
         reserved.append(reservable);
      }
      else if (!reservable->reserve("GUIReserver"))
      {
         message = "Object reserved by " + reservable->reservedFor();
         //qDebug() << "ObjectReserver::reserveForGUI releasing (2)" << s;
         release(reserved, "GUIReserver");
         //KSA replaced empty() with clear(), as empty() is equivalent to isEmpty()
         reserved.clear();
         break;
      }
      else
      {
         reserved.append(reservable);
      }
   }

   return Reservation(reserved, message);
}

void ObjectReserver::release(QList<Reservable *> reserved, QString reserver, bool scriptIsRunning)
{
   Reservable *reservable;

   for (int i = 0; i < reserved.size(); ++i)
   {
      reservable = reserved.at(i);
      reservable->release(reserver);

      if (!scriptIsRunning)
      {
         //qDebug() <<"emitting script release of reserved items - scriptIsRunning = " << scriptIsRunning;
         emit scriptRelease(reservableName(reservable));
      }
   }
   for (int i = 0; i < controlledByGuiList.size(); ++i)
   {
      reservable = controlledByGuiList.at(i);
      reservable->release("GUIReserver");
      if (!scriptIsRunning)
      {
         //qDebug() <<"emitting script release of reserved items - scriptIsRunning = " << scriptIsRunning;
         emit scriptRelease(reservableName(reservable));
      }
   }
}

QString ObjectReserver::reservableName(Reservable *reservable)
{
   QObject *object;
   QString name = "";

   object = dynamic_cast <QObject *> (reservable);
   name = object->property("objectName").toString();

   return name;
}

bool ObjectReserver::getStatus()
{
   return status;
}

bool ObjectReserver::objectInScript(QString objectName, QString script)
{
   bool found = false;

   if (script.contains(objectName + ".") || script.contains(objectName + "->"))
   {
      found = true;
   }

   return found;
}
