#ifndef CHARGESHARING_H
#define CHARGESHARING_H

#include <QDialog>
#include <QtCore>
#include <QDirModel>
#include <QtGui>

namespace Ui {
    class ChargeSharing;
}

class ChargeSharing : public QDialog
{
    Q_OBJECT

public:
    explicit ChargeSharing(QWidget *parent );
    ~ChargeSharing();

private:
    Ui::ChargeSharing *ui;
    QDirModel *model;

private slots:
    void on_buttonBox_accepted();

signals:
    void writeMessage(QString);
    void readData(QString);

public slots:
    void externalCall();
    void externalMessage(QString);

};

#endif // CHARGESHARING_H
