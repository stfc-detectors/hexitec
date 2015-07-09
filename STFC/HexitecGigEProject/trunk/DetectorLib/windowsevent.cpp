#include "windowsevent.h"
#include <QDebug>

WindowsEvent::WindowsEvent(LPCTSTR wname, bool createNotifier, bool manualReset)
{
   this->wname = wname;
   name = QString::fromWCharArray(wname);
   handle = CreateEvent(NULL, manualReset, FALSE, wname);
   if (handle == NULL)
   {
      qDebug() << "Event creation failed";
   }
   notifier = NULL;
   if (createNotifier)
   {
      notifier = new QWinEventNotifier(handle);
      notifier->setEnabled(true);
   }
}

void WindowsEvent::connect1(const QObject *receiver, const char *slot)
{
   if (notifier != NULL)
   {
      connect(notifier, SIGNAL(activated(HANDLE)), receiver, slot);
   }
}

QWinEventNotifier *WindowsEvent::getNotifier()
{
   return notifier;
}

void WindowsEvent::SetEvent1()
{
   int status;
   if (!(status = SetEvent(handle)))
   {
      qDebug() << "Widows event set failed : status = " << status;
   }

}
