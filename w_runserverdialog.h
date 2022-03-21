#ifndef W_RUNSERVERDIALOG_H
#define W_RUNSERVERDIALOG_H

#include "i_observer.h"

#include <QDialog>
#include <QMovie>

namespace Ui {
class w_runServerDialog;
}

class w_runServerDialog : public QDialog, public i_Observer
{
    Q_OBJECT

public:
    explicit w_runServerDialog(QWidget *parent = nullptr);
    ~w_runServerDialog();

public slots:
    void setProcessDescription();

private:

    Ui::w_runServerDialog *ui;

};

#endif // W_RUNSERVERDIALOG_H
