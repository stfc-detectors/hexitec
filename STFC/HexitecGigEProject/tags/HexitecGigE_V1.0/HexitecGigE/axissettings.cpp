#include "axissettings.h"
#include <cmath>

AxisSettings::AxisSettings()
{
    minX	= 0.0;
    maxX	= 1.0;
    minY	= 0.0;
    maxY	= 1.0;
    minZ	= 0.0;
    maxZ	= 1.0;
    minSummedY	= 0.0;
    maxSummedY	= 1.0;
    minTicks = 3;
    maxTicks = 8;
    numXTicks = 5;
    numYTicks = 5;
    numZTicks = 3;
    autoScaleX = true;
    autoScaleY = true;
    autoScaleZ = true;
    autoScaleSummedY = true;
}

AxisSettings::AxisSettings(double minX, double maxX, double minY, double maxY,
                           double minSummedY, double maxSummedY)
{
    this->minX = minX;
    this->maxX = maxX;
    this->minY = minY;
    this->maxY = maxY;
    this->minSummedY = minSummedY;
    this->maxSummedY = maxSummedY;
    minZ = 0.0;
    maxZ = 1.0;
    minTicks = 3;
    maxTicks = 8;
    numXTicks = 5;
    numYTicks = 5;
    numZTicks = 3;
    autoScaleX = false;
    autoScaleY = false;
    autoScaleZ = true;
    autoScaleSummedY = false;
}

void AxisSettings::scroll(int dx, int dy, int dz)
{
    double stepX = spanX() / numXTicks;
    minX += dx * stepX;
    maxX += dx * stepX;

    double stepY = spanY() / numYTicks;
    minY += dy * stepY;
    maxY += dy * stepY;

    double stepZ = spanZ() / numZTicks;
    minZ += dz * stepZ;
    maxZ += dz * stepZ;

    double stepSummedY = spanSummedY() / numYTicks;
    minSummedY += dy * stepSummedY;
    maxSummedY += dy * stepSummedY;
}

//////////////////////////////////////////////////////////////////////////

void AxisSettings::adjust()
{
    /* These first two achieve nothing currently.
     * adjustAxis(minX, maxX, numXTicks);
     * adjustAxis(minY, maxY, numYTicks); */
    adjustAxis(minX, maxX, numXTicks);
    adjustAxis(minY, maxY, numYTicks);
    adjustAxis(minZ, maxZ, numZTicks);
}

void AxisSettings::adjustAxis(double &min, double &max, int &numTicks)
{
//    const int minTicks = 4;

    double grossStep = (max - min) / minTicks;
    double step = pow(10.0, floor(log10(grossStep)));

    if (5 * step < grossStep) {
        step *= 5;
    } else if (2 * step < grossStep) {
        step *= 2;
    }

    numTicks = int(ceil(max / step) - floor(min / step));
    if (numTicks < minTicks)
        numTicks = minTicks;
    if (numTicks > maxTicks)
        numTicks = maxTicks;
    /* Removed to improve sensitivity during adjustment of limits using
     * colour editor. Part of Matt's tidying work.
     * min = floor(min / step) * step;
     * max = ceil(max / step) * step;*/
}

void AxisSettings::setMinTicks(int value)
{
    minTicks = value;
}

void AxisSettings::setMaxTicks(int value)
{
    maxTicks = value;
}

double AxisSettings::spanSummedY()
{
   return maxSummedY - minSummedY;
}
