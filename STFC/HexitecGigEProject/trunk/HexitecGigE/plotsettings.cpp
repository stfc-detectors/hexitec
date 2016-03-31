/*
 PlotSettings class for 2Easy code
 - yet to be implemented
 This has been lifted from Taha's code

 S D M Jacques 02 May 2011
*/
#include "plotsettings.h"
#include <cmath>

PlotSettings::PlotSettings()
{
    minX = 0.0;
    maxX = 10.0;//voxDataPointsNum;
    minY = 0.0;
    maxY = 10.0;
    minSummedY = 0.0;
    maxSummedY = 10.0;
    numXTicks = 10;
    numYTicks = 10;
}

void PlotSettings::scroll(int dx, int dy)
{
    double stepX = spanX() / numXTicks;
    minX += dx * stepX;
    maxX += dx * stepX;

    double stepY = spanY() / numYTicks;
    minY += dy * stepY;
    maxY += dy * stepY;

    double stepSummedY = spanSummedY() / numYTicks;
    minSummedY += dy * stepSummedY;
    maxSummedY += dy * stepSummedY;
}

void PlotSettings::adjust()
{
    adjustAxis(minX, maxX, numXTicks);
    adjustAxis(minY, maxY, numYTicks);
}

void PlotSettings::adjustAxis(double &min, double &max, int &numTicks)
{
    const int MinTicks = 4;
    double grossStep = (max - min) / MinTicks;
    double step = pow(10.0, floor(log10(grossStep)));

    if (5 * step < grossStep) {
        step *= 5;
    } else if (2 * step < grossStep) {
        step *= 2;
    }

    numTicks = int(ceil(max / step) - floor(min / step));
    if (numTicks < MinTicks)
        numTicks = MinTicks;
    min = floor(min / step) * step;
    max = ceil(max / step) * step;
}
