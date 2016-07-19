#ifndef MOTIONCONTROLFORM_H
#define MOTIONCONTROLFORM_H

//Fix MSVC 2013 compiler complaint:
#define TRUE 1
#define FALSE 0

#include <QMainWindow>
#include <QAbstractTableModel>
#include <QTableView>
#include <QWidget>
#include <QString>
#include <QHash>
#include <QList>
#include <string>
#include "motormodel.h"
#include "motordelegate.h"
#include "reservation.h"

namespace Ui {
    class MotionControlForm;
}

class MotionControlForm : public QWidget
{
    Q_OBJECT

public:
    explicit MotionControlForm(QWidget *parent = 0);
   ~MotionControlForm();
   QMainWindow *getMainWindow();

private:
    Ui::MotionControlForm *ui;
    QMainWindow *mainWindow;
    QHash<QString, int> drivePosition;
    QHash<QString, bool> driveAbsolute;
    char *toCharStar(QString qstr);
    int addRow();
    void initialiseForm();
    MotorModel *motorModel;
    QList <Motor *> motors;
    QList <QObject *> requestedMotorsList();
    void updateOutput(QString message);

private slots:
    void initialisePressed();
    void drivePressed();
    void stopPressed();

signals:
    void addObject(QObject *object, bool scripting = TRUE, bool gui = TRUE);
};

#endif // MOTIONCONTROLFORM_H
