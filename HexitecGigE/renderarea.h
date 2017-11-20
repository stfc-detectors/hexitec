/*
RenderArea class for 2Easy code
- this is the surface mapping widget
S D M Jacques 02 May 2011
*/

#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <sarray.h>
#include "axissettings.h"
#include "colormap.h"
#include "colormapeditor.h"
#include "slice.h"

class RenderArea : public QWidget
{
    Q_OBJECT

private:
   bool showColorBars;
public:
    RenderArea(QWidget *parent = 0);

    AxisSettings axes;   // privatise ??
    ColorMap colorMap;
    ColorMapEditor *colorMapEditor;

    Q_INVOKABLE bool isDynamic() {return dynamicState;}
    void setDynamicStateOn() {dynamicState = true;}
    void setDynamicStateOff() {dynamicState = false;}
    void setMouseEnabled(bool enabled);
    void setOffset(int, int);
    void setCellSize(int);
    void setCellSize(int, int);
    QPoint getCellSize() {QPoint p; p.setX(cellSizeX); p.setY(cellSizeY);return p;}
    void changeScaleValue(int);
    Q_INVOKABLE void axesOn() { showAxes = true; }
    Q_INVOKABLE void axesOff() { showAxes = false; }
    Q_INVOKABLE void gridOn() { showGridLines = true; }
    Q_INVOKABLE void gridOff() { showGridLines = false; }
    Q_INVOKABLE void zoomOn() { zoomState = true ;}
    Q_INVOKABLE void zoomOff() { zoomState = false ;}
    Q_INVOKABLE void colorBarOn () {colorBar = true;}
    Q_INVOKABLE void colorBarOff () {colorBar = false;}
    Q_INVOKABLE void colorBarToggle() {colorBar = !colorBar;}
    Q_INVOKABLE void Hexitec_logoOn() {showHexitec_Logo = true;}
    Q_INVOKABLE void Hexitec_logoOff() {showHexitec_Logo = false;}
    Q_INVOKABLE void toolDisplayOn() {showTools = true;}
    Q_INVOKABLE void toolDisplayOff() {showTools = false;}
    Q_INVOKABLE void setMinZ(double);
    Q_INVOKABLE void setMaxZ(double);
    Q_INVOKABLE void setLimZ(double, double);
    Q_INVOKABLE bool isHeld() { return holdState; }
    Q_INVOKABLE void holdOn() { holdState = true; }
    Q_INVOKABLE void holdOff() { holdState = false; }
    Q_INVOKABLE void titleOn() { showTitle = true; }
    Q_INVOKABLE void titleOff() { showTitle = false; }
    Q_INVOKABLE void title(QString value) { plotTitle = value;}
    Q_INVOKABLE void xLabel(QString value) { xPlotLabel = value; }
    Q_INVOKABLE void yLabel(QString value) { yPlotLabel = value; }
    QString title() { return plotTitle; }
    QString xLabel() { return xPlotLabel; }
    QString yLabel() { return yPlotLabel; }
    void paintWidget(QPainter &);
    bool insideRectArea(QRect, QPointF);
    int zMode; //should be set as private at some point
    double minData, maxData, meanData, sumData, sigma, sigma2, nMean, nSigma;
    void autoScaleZ();
    void stats();
    void renderSum(int min, int max, Slice *slice);
    void renderSum(Slice *slice);
    void renderChannel(double value, Slice *slice);
    void setColorBar(bool colorBar);
    SArray <double> imageData;
    void setImageData(SArray <double> imageData);
    AxisSettings getAxes();

signals:
    void updatePlotter();
    void updatePlotter(QPoint, bool);
    void updatePlotter(QVector <QPoint> &, bool);
    void updatePlotter(QVector<double> xData, double *summedImage, int numberOfBins);
    void writeMessage(QString);
    void writeWarning(QString);
    void writeError(QString);

public slots:
    Q_INVOKABLE void zoomIn();
    Q_INVOKABLE void zoomOut();
    Q_INVOKABLE void lockCAxis();
    Q_INVOKABLE void gridToggle(){showGridLines = !showGridLines; update();}
    Q_INVOKABLE void holdPlot() {holdState = !holdState; selectedPixels.resize(0); update();}
    void testDevelopment();
    bool overPixel() {return overPixelState;}
    bool overPixel(QPoint);
    void cycleColorMap();
    int width() {return(widthValue);}
    int height() {return(heightValue);}
    void setZMode(int);
    void setNSigma(double);
    void setNMean(double);
    void setMinimumColour(int);
    void setMaximumColour(int);
    void colorBarsToggle();

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);
    void dropEvent(QDropEvent *de);
    void dragEnterEvent(QDragEnterEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    bool dynamicState;
    bool mouseEnabled;
    bool overPixelState;
    bool overColorBarState;
    int cellSizeX;
    int cellSizeY;
    int offsetX;
    int offsetY;
    QVector <QPoint> selectedPixels;
    QPoint currentPixel;
    int currentDisplaySlice;

    bool showAxes;
    bool zoomState;
    bool showGridLines;
    bool showRubberBand;
    bool showHexitec_Logo;
    bool showTools;
    bool mouseButtonPressed;
    bool mouseDoubleClicked;
    bool holdState;
    bool colorBar;
    bool showTitle;

    QRect rubberBandRect;
    QRect axesBox;
    QRect colorBarBox;
    QString plotTitle;
    QString xPlotLabel;
    QString yPlotLabel;

    int widthValue;
    int heightValue;
    double maxColor;


};

#endif // RENDERAREA_H
