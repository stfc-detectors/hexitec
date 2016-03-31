/*
 PlotSettings class for 2Easy code
 - yet to be implemented
 This has been lifted from Taha's code

 S D M Jacques 02 May 2011
*/
#ifndef PLOTSETTINGS_H
#define PLOTSETTINGS_H

class PlotSettings
{
public:
    PlotSettings();

    void scroll(int dx, int dy);
     void adjust();
     double spanX() const { return maxX - minX; }
     double spanY() const { return maxY - minY; }
     double spanSummedY() const { return maxSummedY - minSummedY; }
     double minX;
     double maxX;
     double minY;
     double maxY;
     double minSummedY;
     double maxSummedY;
     int numXTicks;
     int numYTicks;

 private:
     static void adjustAxis(double &min, double &max, int &numTicks);
};

#endif // PLOTSETTINGS_H
