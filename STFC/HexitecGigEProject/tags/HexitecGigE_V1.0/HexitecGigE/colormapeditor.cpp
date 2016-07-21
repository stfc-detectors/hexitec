#include "colormapeditor.h"
#include "ui_colormapeditor.h"
#include "parameters.h"

ColorMapEditor::ColorMapEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorMapEditor)
{
    ui->setupUi(this);
    ui->nSigma->setValue(1.5);
    ui->nMean->setValue(2.0);

    resetManualColourLimits();
}

void ColorMapEditor::resetManualColourLimits()
{
    QString twoEasyFilename = Parameters::twoEasyIniFilename;

    QSettings settings(QSettings::UserScope, "TEDDI", "HexitecGigE");
    if (settings.contains("hexitecGigEIniFilename"))
    {
       twoEasyFilename = settings.value("hexitecGigEIniFilename").toString();
    }

    twoEasyIniFile = new IniFile(twoEasyFilename);
    int minimumColour = twoEasyIniFile->getInt("Visualisation/Colour_Minimum");
    int maximumColour = twoEasyIniFile->getInt("Visualisation/Colour_Maximum");
    ui->minimumColour->setValue(minimumColour);
    ui->maximumColour->setValue(maximumColour);
}

ColorMapEditor::~ColorMapEditor()
{
    delete ui;
}

void ColorMapEditor::on_minimumColourSlider_valueChanged(int minimum)
{
    setMinimumColour(minimum);
    ui->minimumColour->setValue(minimum);
}

void ColorMapEditor::on_maximumColourSlider_valueChanged(int maximum)
{
    setMaximumColour(maximum);
    ui->maximumColour->setValue(maximum);
}

void ColorMapEditor::on_minimumColour_valueChanged(int minimum)
{
    setMinimumColour(minimum);
    ui->minimumColourSlider->setValue(minimum);
}

void ColorMapEditor::on_maximumColour_valueChanged(int maximum)
{
    setMaximumColour(maximum);
    ui->maximumColourSlider->setValue(maximum);
}

void ColorMapEditor::on_zMode_0_clicked()
{
    setZMode(0);
}

void ColorMapEditor::on_zMode_1_clicked()
{
    setZMode(1);
}

void ColorMapEditor::on_zMode_2_clicked()
{
    setZMode(2);
}

void ColorMapEditor::on_nMean_valueChanged(double)
{
    setNMean(ui->nMean->value());
}

void ColorMapEditor::on_closeButton_clicked()
{
    this->hide();
}

void ColorMapEditor::on_manualColourReset_clicked()
{
    resetManualColourLimits();
}

void ColorMapEditor::on_nSigma_valueChanged(double)
{
    setNSigma(ui->nSigma->value());
}
