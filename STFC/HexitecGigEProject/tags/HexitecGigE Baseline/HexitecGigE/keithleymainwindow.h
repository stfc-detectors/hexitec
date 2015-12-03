#ifndef KEITHLEYMAINWINDOW_H
#define KEITHLEYMAINWINDOW_H

#include <QMainWindow>
#include "plotter.h"

namespace Ui {
class KeithleyMainWindow;
}

class KeithleyMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit KeithleyMainWindow(QWidget *parent = 0);
    ~KeithleyMainWindow();

     Plotter *plotter;
private:
    Ui::KeithleyMainWindow *ui;
};

#endif // KEITHLEYMAINWINDOW_H
