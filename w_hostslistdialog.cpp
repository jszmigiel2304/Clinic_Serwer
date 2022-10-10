#include "w_hostslistdialog.h"
#include "ui_w_hostslistdialog.h"

w_HostsListDialog::w_HostsListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_HostsListDialog)
{
    ui->setupUi(this);
}

w_HostsListDialog::~w_HostsListDialog()
{
    delete ui;
}

QMap<QString, QVariant> w_HostsListDialog::getServerProperties()
{
    QMap<QString, QVariant> map = this->watchedObjectsList["server"]->ShareProperties("hostsOnly");

    return map;
}

void w_HostsListDialog::update()
{
    this->updateHostsList();
    this->hostsGroupBoxList.clear();

    ui->hostListScrollAreaContent->setGeometry(0,0,440, 40 * this->hosts.count());

    QListIterator<c_ClientConnection *> iterator(this->hosts);

    while(iterator.hasNext())
    {
        iterator.next();
    }

}

void w_HostsListDialog::updateHostsList()
{
    this->hosts.clear();
    this->hosts = (dynamic_cast<c_ClinicTcpServer *>(this->watchedObjectsList["server"]))->getHostsList();
}

void w_HostsListDialog::on_refreshButton_clicked()
{
    this->update();
}
