#include "pixelmanipulationform.h"
#include "ui_pixelmanipulationform.h"

PixelManipulationForm::PixelManipulationForm(QWidget *parent) :
   QWidget(parent),
   ui(new Ui::PixelManipulationForm)
{
   ui->setupUi(this);
   connectSignals();
}

PixelManipulationForm::~PixelManipulationForm()
{
   delete ui;
}

void PixelManipulationForm::connectSignals()
{
   connect(ui->pixelAddition, SIGNAL(clicked(bool)), this, SLOT(pixelAdditionClicked(bool)));
}

void PixelManipulationForm::updatePixelList(QString newPixel)
{
   if (newPixel == NULL)
   {
      ui->pixelList->clear();
   }
   else
   {
      ui->pixelList->addItem(newPixel);
   }
}

void PixelManipulationForm::pixelAdditionClicked(bool pixelAdditionOn)
{
   emit pixelAdditionChanged(pixelAdditionOn);
}

