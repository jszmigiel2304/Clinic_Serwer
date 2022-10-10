#ifndef W_DATABASECONNECTIONSCONFIGURATIONDIALOG_H
#define W_DATABASECONNECTIONSCONFIGURATIONDIALOG_H

#include "i_observer.h"
#include "c_settingscontroller.h"
#include "m_databasedrivers.h"
#include "c_myfiles.h"

#include <QPushButton>
#include <QDialog>
#include <QCloseEvent>
#include <QMap>

namespace Ui {
class w_DatabaseConnectionsConfigurationDialog;
}

class w_DatabaseConnectionsConfigurationDialog : public QDialog, public i_Observer
{
    Q_OBJECT

public:
    explicit w_DatabaseConnectionsConfigurationDialog(QWidget *parent = 0);
    ~w_DatabaseConnectionsConfigurationDialog();
    void update();
    void shareDbContrPointer();

public slots:
    void settingsChanged(bool);
    void settingsChanged(QString);

private:
    Ui::w_DatabaseConnectionsConfigurationDialog *ui;
    bool isChanged;

private slots:
    void okButtonPressed();
    void resetButtonPressed();
    void applyButtonPressed();
    void closeEvent(QCloseEvent * e);

};

#endif // W_DATABASECONNECTIONSCONFIGURATIONDIALOG_H
