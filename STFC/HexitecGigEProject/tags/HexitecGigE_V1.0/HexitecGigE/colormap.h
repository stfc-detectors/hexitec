#ifndef COLORMAP_H
#define COLORMAP_H

#include <QBrush>
#include <QPen>
#include <QLinearGradient>
#include <QPainter>
#include <QWidget>



class ColorMap
{
public:
    ColorMap();
    QColor color(double value);
    void nextColorMap();
    QLinearGradient gradient;
    double opacity;
    void Gray();
    void Jet();
    void BlueHighLights();
    void GreenHighLights();
    void RedHighLights();
    void Furnace();
    void Ice();
    void PinkReverse();

private:
    QRect virtualBox;
    QImage gradientImage;
    int colorMapCode;
    void clearGradient();
    void update();

};

#endif // COLORMAP_H
