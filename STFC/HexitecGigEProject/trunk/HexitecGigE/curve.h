#ifndef CURVE_H
#define CURVE_H

#include <QObject>
#include <QVector>
#include <QPointF>
#include <QString>
#include <QPen>
#include <QColor>

class Curve : public QObject
{
    Q_OBJECT
public:
    explicit Curve(QObject *parent = 0);
//    QVector <QPointF> pointData; // maybe should be private - test speed
    QVector <double> xData; // used short term intend to switch to array of QPointF
    QVector <double> yData; // used short term intend to switch to array of QPointF
    void stats();
    void stats(double, double);
//    void stats(double, double); // not yet implemented
//    double getSumYData() { return sumYData; }
//    double getMaxYData() { return maxYData; }

    double minXData;
    double maxXData;
    double minYData;
    double maxYData;
    double sumXData;
    double sumYData;
    QPen curvePen;
    double offset;
    void setHold(bool _holdState) {holdState = _holdState;}
    bool isHeld() {return holdState;}   
    void legends(QString imageName, QPoint pixel, QColor color);

private:
    QString curveName;
    QString curveType;
    bool holdState;
    QString imageName;
    QPoint pixel;
    QColor color;

};

#endif // CURVE_H
