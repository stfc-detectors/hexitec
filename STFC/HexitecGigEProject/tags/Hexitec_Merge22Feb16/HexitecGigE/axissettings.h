#ifndef AXISSETTINGS_H
#define AXISSETTINGS_H

class AxisSettings
{
public:
    AxisSettings();
    AxisSettings(double minX, double maxX, double minY, double maxY);

public:
    void scroll(int dx, int dy, int dz);
    void adjust();
    double spanX() const { return maxX - minX; }
    double spanY() const { return maxY - minY; }
    double spanZ() const { return maxZ - minZ; }
    double minX;
    double maxX;
    double minY;
    double maxY;
    double minZ;
    double maxZ;
    bool autoScaleX;
    bool autoScaleY;
    bool autoScaleZ;
    void setMinTicks(int);
    void setMaxTicks(int);
    int numXTicks;
    int numYTicks;
    int numZTicks;

private:
    void adjustAxis(double &min, double &max, int &numTicks);
    int minTicks;
    int maxTicks;
 };

#endif // AXISSETTINGS_H
