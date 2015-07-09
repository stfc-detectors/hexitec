#ifndef IMAGEACQUIRER_H
#define IMAGEACQUIRER_H

#include <QObject>
#include "gigedetector.h"

class ImageAcquirer : public QObject
{
   Q_OBJECT
public:
   ImageAcquirer(QObject *parent = 0);
   void setDetector(GigEDetector *detector);

private:
   QThread *imageAcquirerThread;
   GigEDetector *detector;

signals:

public slots:
   void handleExecuteAcquireImages();
};

#endif // IMAGEACQUIRER_H
