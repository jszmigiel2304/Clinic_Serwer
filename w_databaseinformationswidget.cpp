#include "w_databaseinformationswidget.h"
#include "ui_w_databaseinformationswidget.h"

w_DatabaseInformationsWidget::w_DatabaseInformationsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::w_DatabaseInformationsWidget)
{
    ui->setupUi(this);

    ui->l_address->setStyleSheet("font: bold;");
    ui->l_port->setStyleSheet("font: bold;");
    ui->l_dbName->setStyleSheet("font: bold;");
    ui->l_dbStatus->setStyleSheet("font: bold;");

    this->timer = new QTimer(this);
    connect(
            this->timer,
            SIGNAL(timeout()),
            this,
            SLOT(refresh())
            );

    this->timer->start(5000);
}

w_DatabaseInformationsWidget::~w_DatabaseInformationsWidget()
{
    this->timer->deleteLater();
    delete ui;
}

void w_DatabaseInformationsWidget::setName(QString dbName)
{
    this->dbName = dbName;
}

void w_DatabaseInformationsWidget::refresh()
{
    ui->databseInfoGroupBox->setTitle(this->dbName);

    QMap<QString, QVariant> map = this->GetWatchedObjectProperties("databaseController" , "basicOnly");

    if(this->dbName == "Authorization database")
    {
        ui->f_dbStatus->setText(map["authDbStatus"].toBool() ? "Connected" : "Disconnected");
        ui->f_address->setText(map["authAddress"].toString());
        ui->f_port->setText(map["authPort"].toString());
        ui->f_Name->setText(map["authDbName"].toString());
    }

    if(this->dbName == "Clinic database")
    {
        ui->f_dbStatus->setText(map["clinicDbStatus"].toBool() ? "Connected" : "Disconnected");
        ui->f_address->setText(map["clinicAddress"].toString());
        ui->f_port->setText(map["clinicPort"].toString());
        ui->f_Name->setText(map["clinicDbName"].toString());
    }


    if (ui->f_dbStatus->text() == "Connected")
        ui->f_dbStatus->setStyleSheet("color: green");
    else
        ui->f_dbStatus->setStyleSheet("color: red");
}
