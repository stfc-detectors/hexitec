#ifndef GENERALHXTGENERATOR_H
#define GENERALHXTGENERATOR_H

#include <QObject>
#include <QDebug>

class GeneralHxtGenerator : public QObject
{
   Q_OBJECT

public:
   GeneralHxtGenerator();

protected:
   unsigned int *histogram;
   unsigned int binStart;
   unsigned int binEnd;
   unsigned int binWidth;

public slots:
   void handleEnqueuePixelEnergy(double *pixelEnergy);

};

#endif // GENERALHXTGENERATOR_H
