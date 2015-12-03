#include "colormap.h"
#include <math.h>


ColorMap::ColorMap()
{
    virtualBox = QRect(0,0,1,1000);
    gradientImage =  QImage(1,1000,QImage::Format_ARGB32);
    colorMapCode = 0;
    Gray();
}

void ColorMap::update()
{
    QPainter painter (&gradientImage);
    painter.setBrush(gradient);
    painter.drawRect(virtualBox.adjusted(-1, -1, 0, 0));
}

QColor ColorMap::color(double value)
{
    value*=1000.0;
    int v = (int) floor(value+0.5);

    if (v < 0)
        v = 0;

    if (v > 999)
        v = 999;

    return(gradientImage.pixel(0,v));
}

void ColorMap::nextColorMap()
{
    colorMapCode++;

    if (colorMapCode > 7)
        colorMapCode = 0;
    if (colorMapCode == 0) Gray();
    if (colorMapCode == 1) Jet();
    if (colorMapCode == 2) Furnace();
    if (colorMapCode == 3) GreenHighLights();
    if (colorMapCode == 4) RedHighLights();
    if (colorMapCode == 5) Ice();
    if (colorMapCode == 6) BlueHighLights();
    if (colorMapCode == 7) PinkReverse();
}

void ColorMap::Gray()
{
    clearGradient();
    gradient.setColorAt(0, Qt::black);
    gradient.setColorAt(1, Qt::white);
    opacity = 0.8;
    update();
}

void ColorMap::Jet()
{
    clearGradient();
    gradient.setColorAt(0, Qt::darkBlue);
    gradient.setColorAt(0.1, Qt::blue);
    gradient.setColorAt(0.40, Qt::cyan);
    gradient.setColorAt(0.50, Qt::green);
    gradient.setColorAt(0.60, Qt::yellow);
    gradient.setColorAt(0.7, QColor(255,165,0));
    gradient.setColorAt(0.9, Qt::red);
    gradient.setColorAt(1, Qt::darkRed);
    opacity = 0.8;
    update();
}

void ColorMap::BlueHighLights()
{
    clearGradient();
    gradient.setColorAt(1, Qt::white);
    gradient.setColorAt(0.8, Qt::blue);
    gradient.setColorAt(0.5, Qt::darkBlue);
    gradient.setColorAt(0, Qt::black);
    opacity = 1.0;
    update();
}

void ColorMap::GreenHighLights()
{
    clearGradient();
    gradient.setColorAt(1, Qt::white);
    gradient.setColorAt(0.8, Qt::green);
    gradient.setColorAt(0.5, Qt::darkGreen);
    gradient.setColorAt(0, Qt::black);
    opacity = 1.0;
    update();
}

void ColorMap::RedHighLights()
{
    clearGradient();
    gradient.setColorAt(1, Qt::white);
    gradient.setColorAt(0.8, Qt::red);
    gradient.setColorAt(0.5, Qt::darkRed);
    gradient.setColorAt(0, Qt::black);
    opacity = 1.0;
    update();
}

void ColorMap::Furnace()
{
    clearGradient();
    gradient.setColorAt(1, Qt::white);
    gradient.setColorAt(0.7, Qt::yellow);
    gradient.setColorAt(0.4, Qt::red);
    gradient.setColorAt(0.2, Qt::darkRed);
    gradient.setColorAt(0, Qt::black);
    opacity = 0.8;
    update();
}

void ColorMap::Ice()
{
    clearGradient();
    gradient.setColorAt(1, Qt::white);
    //gradient.setColorAt(0.7, QColor(0,168,107));//jacde
    gradient.setColorAt(0.8, QColor(80,200,120));
    gradient.setColorAt(0.2, Qt::darkBlue);
    gradient.setColorAt(0, Qt::black);
    opacity = 0.8;
    update();
}

void ColorMap::PinkReverse()
{
    clearGradient();
    gradient.setColorAt(1, Qt::black);
    gradient.setColorAt(0.5, Qt::magenta);
    gradient.setColorAt(0.3, Qt::darkMagenta);
    gradient.setColorAt(0, Qt::white);
    opacity = 0.5;
    update();
}


void ColorMap::clearGradient()
{
    gradient = QLinearGradient();
    gradient.setStart(virtualBox.topLeft());
    gradient.setFinalStop(virtualBox.bottomLeft());
}
