#ifndef SETAXESDIALOG_H
#define SETAXESDIALOG_H

#include <QDialog>
#include "QDoubleValidator"

namespace Ui {
class SetAxesDialog;
}

class SetAxesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetAxesDialog(QWidget *parent = 0);
    ~SetAxesDialog();
    void initializeAxes(double minX, double maxX, double minY, double maxY,
                        double minSummedY, double maxSummedY);
    double getMinX();
    double getMaxX();
    double getMinY();
    double getMaxY();    
    double getMinSummedY();
    double getMaxSummedY();
    bool getAutoScale();

private:
    Ui::SetAxesDialog *ui;
    double minX;
    double maxX;
    double minY;
    double maxY;
    double minSummedY;
    double maxSummedY;
    double autoScale;
    QDoubleValidator *validator;
    void enableInputFields(bool enable);

signals:
    void apply();

private slots:
    void resetAutoScalingButtonStateChanged(int checked);
    void applyButtonClicked();
    void minXChanged(QString valueString);
    void maxXChanged(QString valueString);
    void minYChanged(QString valueString);
    void maxYChanged(QString valueString);
    void minSumYChanged(QString valueString);
    void maxSumYChanged(QString valueString);

};

#endif // SETAXESDIALOG_H
