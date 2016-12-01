#ifndef IMAGEACQUIRER_H
#define IMAGEACQUIRER_H

#include <QObject>
#include "gigedetector.h"

class ImageAcquirer : public QObject
{
   Q_OBJECT
public:
   ImageAcquirer(QObject *parent = 0);
   ~ImageAcquirer();
   void setDetector(GigEDetector *detector);

private:
   QThread *imageAcquirerThread;
   GigEDetector *detector;   
signals:

public slots:
   void handleExecuteAcquireImages(bool startOfImage = true);
};

#endif // IMAGEACQUIRER_H
