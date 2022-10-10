#ifndef W_HOSTSLISTDIALOG_H
#define W_HOSTSLISTDIALOG_H

#include "i_observer.h"
#include "c_clinictcpserver.h"
#include "c_clientconnection.h"

#include <QDialog>
#include <QGroupBox>

namespace Ui {
class w_HostsListDialog;
}

class w_HostsListDialog : public QDialog, public i_Observer
{
    Q_OBJECT

public:
    explicit w_HostsListDialog(QWidget *parent = 0);
    ~w_HostsListDialog();
    QMap<QString, QVariant> getServerProperties();
    void update();
    void updateHostsList();

private slots:
    void on_refreshButton_clicked();

private:
    Ui::w_HostsListDialog *ui;
    QList<c_ClientConnection *> hosts;
    QList<QGroupBox *> hostsGroupBoxList;

};

#endif // W_HOSTSLISTDIALOG_H
