#ifndef VBOUTOFRANGEDIALOG_H
#define VBOUTOFRANGEDIALOG_H

#include <QDialog>

namespace Ui {
class VBOutOfRangeDialog;
}

class VBOutOfRangeDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit VBOutOfRangeDialog(QWidget *parent = 0);
    ~VBOutOfRangeDialog();
    
private:
    Ui::VBOutOfRangeDialog *ui;
};

#endif // VBOUTOFRANGEDIALOG_H
