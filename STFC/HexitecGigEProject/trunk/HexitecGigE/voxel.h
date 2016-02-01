/*
 Voxel class for 2Easy code

 S D M Jacques 24th February 2011
*/
#ifndef VOXEL_H
#define VOXEL_H

#include <QObject>
#include <QVector>
#include <float.h>

class Voxel : public QObject
{
    Q_OBJECT
public:
    Voxel();
    Voxel(int size, double value = 0.0);
    ~Voxel();
    QVector <double> contentXData;
    QVector <double> contentYData;

    double getMax();
    double getMean();
    double getMin();
    double getSum();
    void stats();
    bool readXY(QString fileName);

    Voxel *add(Voxel *anOtherVoxel);
    Voxel *add(double value);
    Voxel *multiply(double value);

private:
    double maxY;
    double meanY;
    double minY;
    double sumY;
};

#endif // VOXEL_H
