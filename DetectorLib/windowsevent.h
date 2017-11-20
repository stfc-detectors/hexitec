#ifndef WINDOWSEVENT_H
#define WINDOWSEVENT_H
#define NOMINMAX
#include <QObject>
#include <Windows.h>
#include <QString>
#include <QWinEventNotifier>

class WindowsEvent : public QObject
{
   Q_OBJECT
public:
   WindowsEvent(LPCTSTR wname, bool createNotifier, bool manualReset = false);
   void connect1(const QObject *receiver, const char *slot);
   QString name;
   LPCTSTR wname;
   HANDLE handle;
   QWinEventNotifier *notifier;
   QWinEventNotifier *getNotifier();
   void SetEvent1();
};

#endif // WINDOWSEVENT_H
