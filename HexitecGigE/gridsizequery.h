#ifndef GRIDSIZEQUERY_H
#define GRIDSIZEQUERY_H

#include <QDialog>

namespace Ui {
    class gridSizeQuery;
}

class gridSizeQuery : public QDialog
{
    Q_OBJECT

public:
    explicit gridSizeQuery(QWidget *parent = 0);
    ~gridSizeQuery();
    void setSize(int,int);
    int numberOfRows;
    int numberOfColumns;

private:
    Ui::gridSizeQuery *ui;

private slots:
    void on_okButton_clicked();
};

#endif // GRIDSIZEQUERY_H
