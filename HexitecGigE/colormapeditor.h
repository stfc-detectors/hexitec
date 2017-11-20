#ifndef COLORMAPEDITOR_H
#define COLORMAPEDITOR_H

#include <QDialog>
#include "inifile.h"

namespace Ui {
    class ColorMapEditor;
}

class ColorMapEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ColorMapEditor(QWidget *parent = 0);
    ~ColorMapEditor();

private slots:
    void on_zMode_0_clicked();
    void on_zMode_1_clicked();
    void on_zMode_2_clicked();
    void on_nMean_valueChanged(double arg1);
    void on_nSigma_valueChanged(double arg1);
    void on_minimumColour_valueChanged(int minimum);
    void on_maximumColour_valueChanged(int maximum);
    void on_minimumColourSlider_valueChanged(int minimum);
    void on_maximumColourSlider_valueChanged(int maximum);
    void on_closeButton_clicked();
    void on_manualColourReset_clicked();

private:
    Ui::ColorMapEditor *ui;
    IniFile *twoEasyIniFile;
    void resetManualColourLimits();

signals:
    void setZMode(int);
    void setNSigma(double);
    void setNMean(double);
    void setMinimumColour(int);
    void setMaximumColour(int);
};

#endif // COLORMAPEDITOR_H
