#ifndef SHT21CONTROLLER_H
#define SHT21CONTROLLER_H

#include <QObject>
#include <QMutex>
#include <QDebug>

class SHT21Controller : public QObject
{
   Q_OBJECT

public:
   ~SHT21Controller();
   enum measurementType {TEMPERATURE = 0x03, HUMIDITY = 0x05};
   static SHT21Controller *instance(QObject *parent);
   short ReadI2C(unsigned char call);
   void init();
   void close();

private:
   SHT21Controller();
   static SHT21Controller *sht21cInstance;
   bool connected;
//   IOWKIT_HANDLE ioHandle;
   void *ioHandle;
   int numDevices;
   QMutex mutex;

signals:
   void writeError(QString message);
   void writeMessage(QString message);
   void destruction();
};

#endif // SHT21CONTROLLER_H
