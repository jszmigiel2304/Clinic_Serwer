#ifndef W_WINDOWCONFIGURATIONDIALOG_H
#define W_WINDOWCONFIGURATIONDIALOG_H

#include "i_observer.h"
#include "c_settingscontroller.h"
#include "c_myfiles.h"

#include <QDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>

namespace Ui {
class w_windowConfigurationDialog;
}

class w_windowConfigurationDialog : public QDialog, public i_Observer
{
    Q_OBJECT

public:
    explicit w_windowConfigurationDialog(QWidget *parent = 0);
    ~w_windowConfigurationDialog();
    QMap<QString, QVariant> getWindowProperties();
    void update();

private:
    Ui::w_windowConfigurationDialog *ui;

    bool isChanged;


private slots:
    void settingsChanged(bool);
    void settingsChanged(QString);
    void okButtonPressed();
    void resetButtonPressed();
    void applyButtonPressed();
    void closeEvent(QCloseEvent * e);
};

#endif // W_WINDOWCONFIGURATIONDIALOG_H
