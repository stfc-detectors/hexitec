#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
   Q_OBJECT
public:
   explicit MainWindow(QWidget *parent = 0);
private:
   void createMenus();
private slots:
   void accept();
   void reject();
   void printStates();
   void deleteSettings();
};

#endif // MAINWINDOW_H
