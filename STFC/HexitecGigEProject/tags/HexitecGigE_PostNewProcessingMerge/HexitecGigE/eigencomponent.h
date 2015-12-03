/*
 Eigen component class for 2Easy code

 S D M Jacques 31st October 2012
*/
#ifndef EIGENCOMPONENT_H
#define EIGENCOMPONENT_H

#include <QObject>
#include <QVector>
#include <float.h>
#include "sarray.h"

class EigenComponent : public QObject
{
    Q_OBJECT
public:
    // Constructors
    EigenComponent(QObject *parent = 0);
    EigenComponent( int, int, int);

    SArray <double> contentImage;
    QVector <double> contentSpectra;

signals:
    
public slots:
    
private:
    int gridSizeX;
    int gridSizeY;
    int spectrumDataLen;
    void resize();

};

#endif // EIGENCOMPONENT_H
