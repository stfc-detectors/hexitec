#include "curve.h"

Curve::Curve(QObject *parent) :
    QObject(parent)
{
    // pen attributes
    curvePen.setJoinStyle(Qt::BevelJoin);
    curvePen.setCapStyle(Qt::FlatCap);
    curvePen.setStyle(Qt::SolidLine);
    curvePen.setWidth(2.0);
 //   curvePen.setColor();
//    curvePen.setBrush(QColor(150,150,150));
    curvePen.setBrush(QColor(Qt::blue));

    minXData = 0;
    maxXData = 0;
    minYData = 1e10;
    maxYData = -1e10;
    sumXData = 0;
    sumYData = 0;
    offset = 0;

    holdState = false;

}

void Curve::stats()
{
    minXData = xData[0];
    maxXData = xData[0];
    minYData = yData[0];
    maxYData = yData[0];
    sumXData = 0;
    sumYData = 0;

    for (int i = 0 ; i < xData.size(); ++i)
    {
        if (xData[i] > maxXData)
            maxXData = xData[i];
        if (xData[i] < minXData)
            minXData = xData[i];
        if (yData[i]+offset > maxYData)
            maxYData = yData[i]+offset;
        if (yData[i]+offset < minYData)
            minYData = yData[i]+offset;

        sumXData+= xData[i];
        sumYData+= yData[i];
    }

    return;
}

void Curve::stats(double min, double max)
{
    minXData = min;
    maxXData = max;
    minYData = 1e10;
    maxYData = -1e10;
    sumXData = 0;
    sumYData = 0;

    int j;
    for (j = 0 ; j < xData.size(); ++j)
    {
        if (xData[j] > min)
            break;
    }

    for (int i = j ; i < xData.size(); ++i)
    {
        if (xData[i] > max)
            break;
        if (yData[i]+offset > maxYData)
            maxYData = yData[i]+offset;
        if (yData[i]+offset < minYData)
            minYData = yData[i]+offset;
        sumXData+= xData[i];
        sumYData+= yData[i];
    }
    return;
}

void Curve::legends(QString imageName, QPoint pixel, QColor color)
{
    this->imageName = imageName;
    this->pixel = pixel;
    this->color = color;
}
