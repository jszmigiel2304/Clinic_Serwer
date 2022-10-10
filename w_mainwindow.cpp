#include "w_mainwindow.h"
#include "ui_w_mainwindow.h"

w_MainWindow::w_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::w_MainWindow)
{
    ui->setupUi(this);
}

w_MainWindow::w_MainWindow(QMap<QString, QVariant> settings, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::w_MainWindow)
{
    ui->setupUi(this);

    this->createTrayIcon();

    this->startMinimize = settings["startMinimize"].toBool();
    this->minimizeToTrayOnClose = settings["minimizeToTrayOnClose"].toBool();

    ui->actionServerConfigure->setEnabled(true);
    ui->actionServerStart->setEnabled(true);
    ui->actionServerStop->setEnabled(false);
    ui->actionHostsList->setEnabled(true);

    ui->authDbInfoArea->setName("Authorization database");
    ui->clinicDbInfoArea->setName("Clinic database");

    this->trayIcon->show();
}

w_MainWindow::~w_MainWindow()
{
    delete ui;
}

QMap<QString, QVariant> w_MainWindow::ShareProperties(QString sharedData)
{
    QMap<QString, QVariant> map;


    if(sharedData == "all" || sharedData == "basicOnly")
    {
        map.insert("startMinimize", this->startMinimize);
        map.insert("minimizeToTrayOnClose", this->minimizeToTrayOnClose);
    }

    return map;
}

void w_MainWindow::UpdateProperties(QMap<QString, QVariant> map)
{
    this->minimizeToTrayOnClose = map["minimizeToTrayOnClose"].toBool();
    this->startMinimize = map["startMinimize"].toBool();

    emit this->PropertiesChanged();
}

void w_MainWindow::showEvent(QShowEvent *)
{
}

void w_MainWindow::refresh()
{    
    QMap<QString, QVariant> map = this->GetWatchedObjectProperties("server");

    if(map["isListening"].toBool())
    {
        ui->actionServerStart->setEnabled(false);
        ui->actionServerStop->setEnabled(true);
        ui->actionServerConfigure->setEnabled(false);
    }
    else
    {
        ui->actionServerStart->setEnabled(true);
        ui->actionServerConfigure->setEnabled(true);
        ui->actionServerStop->setEnabled(false);
    }
    ui->interfaceInfoArea->refresh();
    ui->serverInfoArea->refresh();
    ui->authDbInfoArea->refresh();
    ui->clinicDbInfoArea->refresh();
}

void w_MainWindow::changeStatusBarMessage(QString msg, int time)
{
    ui->statusBar->showMessage(msg, time);
}

void w_MainWindow::shareServerPointer()
{
    this->ShareWatchedObject("server", dynamic_cast<i_Observer *>(ui->interfaceInfoArea));
    connect(
            dynamic_cast<c_ClinicTcpServer *>(ui->interfaceInfoArea->watchedObjectsList["server"]),
            SIGNAL(PropertiesChanged()),
            ui->interfaceInfoArea,
            SLOT(refresh())
            );

    this->ShareWatchedObject("server", dynamic_cast<i_Observer *>(ui->serverInfoArea));
    connect(
            dynamic_cast<c_ClinicTcpServer *>(ui->interfaceInfoArea->watchedObjectsList["server"]),
            SIGNAL(PropertiesChanged()),
            ui->serverInfoArea,
            SLOT(refresh())
            );
}

void w_MainWindow::shareDbContrPointer()
{
    this->ShareWatchedObject("databaseController", dynamic_cast<i_Observer *>(ui->serverInfoArea));
    connect(
            dynamic_cast<c_MySqlDatabaseController *>(ui->serverInfoArea->watchedObjectsList["databaseController"]),
            SIGNAL(PropertiesChanged()),
            ui->serverInfoArea,
            SLOT(refresh())
            );

    this->ShareWatchedObject("databaseController", dynamic_cast<i_Observer *>(ui->authDbInfoArea));
    connect(
            dynamic_cast<c_MySqlDatabaseController *>(ui->authDbInfoArea->watchedObjectsList["databaseController"]),
            SIGNAL(PropertiesChanged()),
            ui->authDbInfoArea,
            SLOT(refresh())
            );

    this->ShareWatchedObject("databaseController", dynamic_cast<i_Observer *>(ui->clinicDbInfoArea));
    connect(
            dynamic_cast<c_MySqlDatabaseController *>(ui->clinicDbInfoArea->watchedObjectsList["databaseController"]),
            SIGNAL(PropertiesChanged()),
            ui->clinicDbInfoArea,
            SLOT(refresh())
            );
}

void w_MainWindow::MyShow()
{
    if(this->startMinimize)
    {
        QIcon icon(":/actions/icons/appIcon.png");
        this->trayIcon->showMessage("Przychodnia - Serwer", "Zminimalizowano do zasobnika systemowego.", icon ,1500);
    }
    else
        this->show();
}


void w_MainWindow::on_actionServerConfigure_triggered()
{
    if((this->watchedObjectsList["server"]->ShareProperties("basicOnly"))["isListening"].toBool())
    {
        QMessageBox::information(
                this,
                tr("Informacja"),
                tr("Server jest w trybie nasłuchiwania.\nZatrzymaj zanim zaczniesz konfigurację.") );
    } else
    {
        w_serverConfigurationDialog * wnd = new w_serverConfigurationDialog(this);
        //wnd->AddWatchedObject("server", this->watchedObjectsList["server"]);
        this->ShareWatchedObject("server", wnd);
        wnd->update();
        wnd->show();
    }
}

void w_MainWindow::on_actionServerStart_triggered()
{
    (dynamic_cast<c_ClinicTcpServer *>(this->watchedObjectsList["server"]))->runServer();

    ui->actionServerStart->setEnabled(false);
    ui->actionServerStop->setEnabled(true);
    ui->actionServerConfigure->setEnabled(false);

    ui->actionDataBaseConfigure->setEnabled(false);
}

void w_MainWindow::on_actionServerStop_triggered()
{
    (dynamic_cast<c_ClinicTcpServer *>(this->watchedObjectsList["server"]))->stopServer();

    ui->actionServerStart->setEnabled(true);
    ui->actionServerConfigure->setEnabled(true);
    ui->actionServerStop->setEnabled(false);

    ui->actionDataBaseConfigure->setEnabled(true);
}

void w_MainWindow::on_actionWindowConfigure_triggered()
{
        w_windowConfigurationDialog * wnd = new w_windowConfigurationDialog(this);
        //wnd->AddWatchedObject("server", this->watchedObjectsList["server"]);
        wnd->AddWatchedObject("appWindow", dynamic_cast<i_Watched *>(this));
        wnd->update();
        wnd->show();
}

void w_MainWindow::createTrayIcon()
{
    this->trayIcon = new QSystemTrayIcon(this);


    QIcon icon(":/actions/icons/appIcon.png");
    this->trayIcon->setIcon(icon);

    this->trayIconMenu = new QMenu();
    trayIconMenu->addAction(ui->actionOpen);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(ui->actionServerStart);
    trayIconMenu->addAction(ui->actionServerStop);
    trayIconMenu->addAction(ui->actionServerConfigure);
    trayIconMenu->addAction(ui->actionHostsList);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(ui->actionDataBaseConfigure);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(ui->actionClose);

    trayIconMenu->setDefaultAction(ui->actionOpen);


    trayIcon->setContextMenu(trayIconMenu);

    connect(trayIcon, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ), this, SLOT( ontrayIconactivated( QSystemTrayIcon::ActivationReason ) ) );
}

void w_MainWindow::closeEvent(QCloseEvent *event)
{
    if(this->minimizeToTrayOnClose)
    {
        this->hide();

        QIcon icon(":/actions/icons/appIcon.png");
        this->trayIcon->showMessage("Przychodnia - Serwer", "Zminimalizowano do zasobnika systemowego.", icon ,1500);
        event->ignore();
    }
    else
    {        
        this->on_actionServerStop_triggered();
        QIcon icon(":/actions/icons/appIcon.png");
        this->trayIcon->showMessage("Przychodnia - Serwer", "Trwa zamykanie.", icon ,1000);

        if(w_HostsListWindow::Instance().getIsShown())
            w_HostsListWindow::Instance().close();

        if(w_logsWindow::Instance() != nullptr)
            w_logsWindow::Instance()->close();

        event->accept();
    }
}

void w_MainWindow::on_actionOpen_triggered()
{
    this->show();
}

void w_MainWindow::ontrayIconactivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick)
        this->on_actionOpen_triggered();
}

void w_MainWindow::on_actionClose_triggered()
{
    this->minimizeToTrayOnClose = false;
    this->close();
}

void w_MainWindow::on_actionDataBaseConfigure_triggered()
{
    if((this->watchedObjectsList["server"]->ShareProperties("basicOnly"))["isListening"].toBool())
    {
        QMessageBox::information(
                this,
                tr("Informacja"),
                tr("Server jest w trybie nasłuchiwania.\nZatrzymaj zanim zaczniesz konfigurację.") );
    } else
    {
        w_DatabaseConnectionsConfigurationDialog * wnd = new w_DatabaseConnectionsConfigurationDialog(this);
        //wnd->AddWatchedObject("server", this->watchedObjectsList["server"]);
        this->ShareWatchedObject("databaseController", wnd);
        wnd->shareDbContrPointer();
        wnd->update();
        wnd->show();
    }
}

void w_MainWindow::on_actionHostsList_triggered()
{
    this->show();
    this->ShareWatchedObject("server", &w_HostsListWindow::Instance());
    w_HostsListWindow::Instance().update();
    w_HostsListWindow::Instance().show();
    w_HostsListWindow::Instance().setIsShown(true);
    w_HostsListWindow::Instance().activateWindow();
    w_HostsListWindow::Instance().raise();
}
