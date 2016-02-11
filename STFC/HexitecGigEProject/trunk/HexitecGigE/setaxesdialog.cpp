#include "setaxesdialog.h"
#include "ui_setaxesdialog.h"

SetAxesDialog::SetAxesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetAxesDialog)
{
    validator = new QDoubleValidator(this);
    ui->setupUi(this);
    ui->minXInput->setValidator(validator);
    ui->maxXInput->setValidator(validator);
    ui->minYInput->setValidator(validator);
    ui->maxYInput->setValidator(validator);

    connect(ui->resetAutoScalingButton, SIGNAL(stateChanged(int)),this, SLOT(resetAutoScalingButtonStateChanged(int)));
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applyButtonClicked()));
    connect(ui->minXInput, SIGNAL(textEdited(QString)), this, SLOT(minXChanged(QString)));
    connect(ui->maxXInput, SIGNAL(textEdited(QString)), this, SLOT(maxXChanged(QString)));
    connect(ui->minYInput, SIGNAL(textEdited(QString)), this, SLOT(minYChanged(QString)));
    connect(ui->maxYInput, SIGNAL(textEdited(QString)), this, SLOT(maxYChanged(QString)));
}

SetAxesDialog::~SetAxesDialog()
{
    delete ui;
}

void SetAxesDialog::initializeAxes(double minX, double maxX, double minY, double maxY)
{
    this->minX = minX;
    this->maxX = maxX;
    this->minY = minY;
    this->maxY = maxY;
    ui->minXInput->setText(QString::number(minX));
    ui->maxXInput->setText(QString::number(maxX));
    ui->minYInput->setText(QString::number(minY));
    ui->maxYInput->setText(QString::number(maxY));
    autoScale = ui->resetAutoScalingButton->isChecked();
    enableInputFields(!autoScale);
}

void SetAxesDialog::enableInputFields(bool enable)
{
    ui->minXInput->setEnabled(enable);
    ui->maxXInput->setEnabled(enable);
    ui->minYInput->setEnabled(enable);
    ui->maxYInput->setEnabled(enable);
}

double SetAxesDialog::getMinX()
{
    return minX;
}

double SetAxesDialog::getMaxX()
{
    return maxX;
}

double SetAxesDialog::getMinY()
{
    return minY;
}

double SetAxesDialog::getMaxY()
{
    return maxY;
}

void SetAxesDialog::applyButtonClicked()
{
    emit apply();
}

bool SetAxesDialog::getAutoScale()
{
    return autoScale;
}

void SetAxesDialog::resetAutoScalingButtonStateChanged(int checked)
{
    autoScale = checked;
    enableInputFields(!autoScale);
}

void SetAxesDialog::minXChanged(QString valueString)
{
    minX = valueString.toDouble();
}

void SetAxesDialog::maxXChanged(QString valueString)
{
    maxX = valueString.toDouble();
}

void SetAxesDialog::minYChanged(QString valueString)
{
    minY = valueString.toDouble();
}

void SetAxesDialog::maxYChanged(QString valueString)
{
    maxY = valueString.toDouble();
}
