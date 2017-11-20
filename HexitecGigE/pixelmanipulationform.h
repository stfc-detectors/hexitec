#ifndef PIXELMANIPULATIONFORM_H
#define PIXELMANIPULATIONFORM_H

#include <QWidget>

namespace Ui {
class PixelManipulationForm;
}

class PixelManipulationForm : public QWidget
{
   Q_OBJECT

public:
   explicit PixelManipulationForm(QWidget *parent = 0);
   ~PixelManipulationForm();

private:
   Ui::PixelManipulationForm *ui;
   void connectSignals();

public slots:
   void updatePixelList(QString newPixel);

private slots:
   void pixelAdditionClicked(bool pixelAdditionOn);

signals:
   void pixelAdditionChanged(bool);
};

#endif // PIXELMANIPULATIONFORM_H
