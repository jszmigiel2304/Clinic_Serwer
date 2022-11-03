#include "w_serverinformationswidget.h"
#include "ui_w_serverinformationswidget.h"

w_ServerInformationsWidget::w_ServerInformationsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::w_ServerInformationsWidget)
{
    ui->setupUi(this);

    this->timer = new QTimer(this);
    connect(
            this->timer,
            SIGNAL(timeout()),
            this,
            SLOT(refresh())
            );

    timer->start(5000);
}

w_ServerInformationsWidget::~w_ServerInformationsWidget()
{
    this->timer->deleteLater();
    delete ui;
}

void w_ServerInformationsWidget::refresh()
{
    QMap<QString, QVariant> map = this->GetWatchedObjectProperties("server", "basicOnly");
    QMap<QString, QVariant> map2 = this->GetWatchedObjectProperties("databaseController", "basicOnly");

    ui->f_address->setText(map["address"].toString());
    ui->f_serverStatus->setText(map["isListening"].toBool() ? "UP" : "DOWN");
    ui->f_port->setText(map["port"].toString());
    ui->f_HostsNumber->setText(map["hostsNumber"].toString());

    ui->f_AuthDbStatus->setText(map2["authDbStatus"].toBool() ? "Connected" : "Disconnected");
    ui->f_ClinicDbStatus->setText(map2["clinicDbStatus"].toBool() ? "Connected" : "Disconnected");
    //ui->f_HostsNumber->setText();

    if (ui->f_serverStatus->text() == "UP")
        ui->f_serverStatus->setStyleSheet("color: green");
    else
        ui->f_serverStatus->setStyleSheet("color: red");


    if (ui->f_AuthDbStatus->text() == "Connected")
        ui->f_AuthDbStatus->setStyleSheet("color: green");
    else
        ui->f_AuthDbStatus->setStyleSheet("color: red");


    if (ui->f_ClinicDbStatus->text() == "Connected")
        ui->f_ClinicDbStatus->setStyleSheet("color: green");
    else
        ui->f_ClinicDbStatus->setStyleSheet("color: red");

}
