#include "w_interfaceinformationswidget.h"
#include "ui_w_interfaceinformationswidget.h"

w_InterfaceInformationsWidget::w_InterfaceInformationsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::w_InterfaceInformationsWidget)
{
    ui->setupUi(this);

    this->timer = new QTimer(this);
    connect(
            this->timer,
            SIGNAL(timeout()),
            this,
            SLOT(refresh())
            );

    timer->start(15000);
}

w_InterfaceInformationsWidget::~w_InterfaceInformationsWidget()
{
    delete ui;
}

void w_InterfaceInformationsWidget::refresh()
{    
    QMap<QString, QVariant> map = this->GetWatchedObjectProperties("server", "basicOnly");
    QString interfaceName = map["interfaceName"].toString();

    if(interfaceName != "any")
    {
        QNetworkInterface interface;
        foreach (QNetworkInterface i, QNetworkInterface::allInterfaces()) {
            if (i.name() == interfaceName)
                interface = i;
        }

        ui->f_name->setText(interface.name());
        ui->f_humanName->setText(interface.humanReadableName());
        ui->f_mac->setText(interface.hardwareAddress());

        int i = 0;

        if(interface.addressEntries().length() > 1)
        {
            ui->l_ipv4Label->setText("IPv4");
            i = 1;
        } else
        {
            ui->l_ipv4Label->setText("IPv6");
        }

        ui->f_ipv4->setText(interface.addressEntries()[i].ip().toString());
        ui->f_netmask->setText(interface.addressEntries()[i].netmask().toString());
        ui->f_broadcast->setText(interface.addressEntries()[i].broadcast().toString());


        ui->f_canBroadcast->setText((bool) (interface.flags() & QNetworkInterface::CanBroadcast) ? "YES" : "NO");
        ui->f_canMlticast->setText((bool) (interface.flags() & QNetworkInterface::CanMulticast) ? "YES" : "NO");
        ui->f_isUp->setText((bool) (interface.flags() & QNetworkInterface::IsUp) ? "YES" : "NO");
        ui->f_IsRunning->setText((bool) (interface.flags() & QNetworkInterface::IsRunning)? "YES" : "NO");
        ui->f_isLoopBack->setText((bool) (interface.flags() & QNetworkInterface::IsLoopBack) ? "YES" : "NO");
        ui->f_isPointToPoint->setText((bool) (interface.flags() & QNetworkInterface::IsPointToPoint) ? "YES" : "NO");
    }
    else
    {
        ui->f_name->setText("-----");
        ui->f_humanName->setText("-----");
        ui->f_ipv4->setText("-----");
        ui->f_netmask->setText("-----");
        ui->f_broadcast->setText("-----");
        ui->f_mac->setText("-----");
        ui->l_ipv4Label->setText("IPv4");

        ui->f_canBroadcast->setText("-");
        ui->f_canMlticast->setText("-");
        ui->f_isUp->setText("-");
        ui->f_IsRunning->setText("-");
        ui->f_isLoopBack->setText("-");
        ui->f_isPointToPoint->setText("-");
    }


    if (ui->f_canBroadcast->text() == "YES")
        ui->f_canBroadcast->setStyleSheet("color: green");
    else if (ui->f_canBroadcast->text() == "NO")
        ui->f_canBroadcast->setStyleSheet("color: red");
    else
        ui->f_canBroadcast->setStyleSheet("color: black");


    if (ui->f_canMlticast->text() == "YES")
        ui->f_canMlticast->setStyleSheet("color: green");
    else if (ui->f_canMlticast->text() == "NO")
        ui->f_canMlticast->setStyleSheet("color: red");
    else
        ui->f_canMlticast->setStyleSheet("color: black");



    if (ui->f_isLoopBack->text() == "YES")
        ui->f_isLoopBack->setStyleSheet("color: green");
    else if (ui->f_isLoopBack->text() == "NO")
        ui->f_isLoopBack->setStyleSheet("color: red");
    else
        ui->f_isLoopBack->setStyleSheet("color: black");



    if (ui->f_isPointToPoint->text() == "YES")
        ui->f_isPointToPoint->setStyleSheet("color: green");
    else if (ui->f_isPointToPoint->text() == "NO")
        ui->f_isPointToPoint->setStyleSheet("color: red");
    else
        ui->f_isPointToPoint->setStyleSheet("color: black");



    if (ui->f_isUp->text() == "YES")
        ui->f_isUp->setStyleSheet("color: green");
    else if (ui->f_isUp->text() == "NO")
        ui->f_isUp->setStyleSheet("color: red");
    else
        ui->f_isUp->setStyleSheet("color: black");



    if (ui->f_IsRunning->text() == "YES")
        ui->f_IsRunning->setStyleSheet("color: green");
    else if (ui->f_IsRunning->text() == "NO")
        ui->f_IsRunning->setStyleSheet("color: red");
    else
        ui->f_IsRunning->setStyleSheet("color: black");




}

//void w_InterfaceInformationsWidget::refreshInformations(QMap<QString, QVariant> map)
//{
//    QString interfaceName = map["interfaceName"].toString();

//    if (interfaceName != "any")
//    {
//        QNetworkInterface interface;
//        foreach (QNetworkInterface i, QNetworkInterface::allInterfaces()) {
//            if (i.name() == interfaceName)
//                interface = i;
//        }

//        ui->f_name->setText(interface.name());
//        ui->f_humanName->setText(interface.humanReadableName());
//        ui->f_mac->setText(interface.hardwareAddress());

//        int i = 0;

//        if(interface.addressEntries().length() > 1)
//        {
//            ui->l_ipv4Label->setText("IPv4");
//            i = 1;
//        } else
//        {
//            ui->l_ipv4Label->setText("IPv6");
//        }

//        ui->f_ipv4->setText(interface.addressEntries()[i].ip().toString());
//        ui->f_netmask->setText(interface.addressEntries()[i].netmask().toString());
//        ui->f_broadcast->setText(interface.addressEntries()[i].broadcast().toString());

//        ui->f_canBroadcast->setText((bool) (interface.flags() & QNetworkInterface::CanBroadcast) ? "YES" : "NO");
//        ui->f_canMlticast->setText((bool) (interface.flags() & QNetworkInterface::CanMulticast) ? "YES" : "NO");
//        ui->f_isUp->setText((bool) (interface.flags() & QNetworkInterface::IsUp) ? "YES" : "NO");
//        ui->f_IsRunning->setText((bool) (interface.flags() & QNetworkInterface::IsRunning)? "YES" : "NO");
//        ui->f_isLoopBack->setText((bool) (interface.flags() & QNetworkInterface::IsLoopBack) ? "YES" : "NO");
//        ui->f_isPointToPoint->setText((bool) (interface.flags() & QNetworkInterface::IsPointToPoint) ? "YES" : "NO");
//    }
//    else
//    {;
//        ui->f_name->setText("-----");
//        ui->f_humanName->setText("-----");
//        ui->f_ipv4->setText("-----");
//        ui->f_netmask->setText("-----");
//        ui->f_broadcast->setText("-----");
//        ui->f_mac->setText("-----");
//        ui->l_ipv4Label->setText("IPv4");

//        ui->f_canBroadcast->setText("-");
//        ui->f_canMlticast->setText("-");
//        ui->f_isUp->setText("-");
//        ui->f_IsRunning->setText("-");
//        ui->f_isLoopBack->setText("-");
//        ui->f_isPointToPoint->setText("-");
//    }
//}
