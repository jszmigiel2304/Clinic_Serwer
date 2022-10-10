#ifndef W_SERVERCONFIGURATIONDIALOG_H
#define W_SERVERCONFIGURATIONDIALOG_H

#include "i_observer.h"
#include "c_settingscontroller.h"
#include "c_myfiles.h"

#include <QDialog>
#include <QRadioButton>
#include <QNetworkInterface>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>
#include <QIntValidator>
#include <QCloseEvent>

namespace Ui {
class w_serverConfigurationDialog;
}

class w_serverConfigurationDialog : public QDialog, public i_Observer
{
    Q_OBJECT

public:
    explicit w_serverConfigurationDialog(QWidget *parent = nullptr);
    ~w_serverConfigurationDialog();
    QMap<QString, QVariant> getServerProperties();
    void update();


private:
    Ui::w_serverConfigurationDialog *ui;
    QList<QRadioButton *> interfacesRadioButtonList;
    bool isChanged;

private slots:
    void listeningOptionGroupBoxClicked(bool);
    void settingsChanged(bool);
    void settingsChanged(QString);
    void okButtonPressed();
    void resetButtonPressed();
    void applyButtonPressed();
    void closeEvent(QCloseEvent * e);
};

#endif // W_SERVERCONFIGURATIONDIALOG_H
