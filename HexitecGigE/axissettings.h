#ifndef AXISSETTINGS_H
#define AXISSETTINGS_H

class AxisSettings
{
public:
    AxisSettings();
    AxisSettings(double minX, double maxX, double minY, double maxY,
                 double minSummedY, double maxSummedY);

public:
    void scroll(int dx, int dy, int dz);
    void adjust();
    double spanX() const { return maxX - minX; }
    double spanY() const { return maxY - minY; }
    double spanZ() const { return maxZ - minZ; }
    double spanSummedY();
    double minX;
    double maxX;
    double minY;
    double maxY;
    double minZ;
    double maxZ;
    double minSummedY;
    double maxSummedY;
    bool autoScaleX;
    bool autoScaleY;
    bool autoScaleZ;
    bool autoScaleSummedY;
    void setMinTicks(int);
    void setMaxTicks(int);
    int numXTicks;
    int numYTicks;
    int numZTicks;
    int numSummedYTicks;

private:
    void adjustAxis(double &min, double &max, int &numTicks);
    int minTicks;
    int maxTicks;
 };

#endif // AXISSETTINGS_H
