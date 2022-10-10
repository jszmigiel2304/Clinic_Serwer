#include "w_databaseconnectionwidget.h"
#include "ui_w_databaseconnectionwidget.h"

w_DatabaseConnectionWidget::w_DatabaseConnectionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::w_DatabaseConnectionWidget)
{
    ui->setupUi(this);

    this->ui->f_driverName->addItem(SQLITE_VERSION_2_DRIVER);
    this->ui->f_driverName->addItem(SQLITE_VERSION_3_DRIVER);
    this->ui->f_driverName->addItem(POSTGRESQL_DRIVER);
    this->ui->f_driverName->addItem(MYSQL_DRIVER);
    this->ui->f_driverName->addItem(ODBC_MSSQL_DRIVER);

    ui->f_port->setValidator(new QIntValidator(0, 65535, this));

    connect(ui->f_dbName, SIGNAL(textEdited(QString)),
            dynamic_cast<w_DatabaseConnectionsConfigurationDialog *>(this->parent()->parent()), SLOT(settingsChanged(QString)));
    connect(ui->f_driverName, SIGNAL(currentTextChanged(QString)),
            dynamic_cast<w_DatabaseConnectionsConfigurationDialog *>(this->parent()->parent()), SLOT(settingsChanged(QString)));
    connect(ui->f_hostName, SIGNAL(textEdited(QString)),
            dynamic_cast<w_DatabaseConnectionsConfigurationDialog *>(this->parent()->parent()), SLOT(settingsChanged(QString)));
    connect(ui->f_password, SIGNAL(textEdited(QString)),
            dynamic_cast<w_DatabaseConnectionsConfigurationDialog *>(this->parent()->parent()), SLOT(settingsChanged(QString)));
    connect(ui->f_port, SIGNAL(textEdited(QString)),
            dynamic_cast<w_DatabaseConnectionsConfigurationDialog *>(this->parent()->parent()), SLOT(settingsChanged(QString)));
    connect(ui->f_userName, SIGNAL(textEdited(QString)),
            dynamic_cast<w_DatabaseConnectionsConfigurationDialog *>(this->parent()->parent()), SLOT(settingsChanged(QString)));
}

w_DatabaseConnectionWidget::~w_DatabaseConnectionWidget()
{
    delete ui;
}

void w_DatabaseConnectionWidget::update()
{
   QMap<QString, QVariant> map = this->getDbControllerProperties();

   if(this->dbName == "Authorization")
   {
       ui->f_dbName->setText( map["authDbName"].toString() );
       ui->f_driverName->setCurrentIndex( ui->f_driverName->findText(map["authDriver"].toString()) );
       ui->f_hostName->setText( map["authAddress"].toString() );
       ui->f_password->setText( map["authPassword"].toString() );
       ui->f_port->setText( map["authPort"].toString() );
       ui->f_userName->setText( map["authUserName"].toString() );
   }
   else if (this->dbName == "Clinic")
   {
       ui->f_dbName->setText( map["clinicDbName"].toString() );
       ui->f_driverName->setCurrentIndex( ui->f_driverName->findText(map["clinicDriver"].toString()) );
       ui->f_hostName->setText( map["clinicAddress"].toString() );
       ui->f_password->setText( map["clinicPassword"].toString() );
       ui->f_port->setText( map["clinicPort"].toString() );
       ui->f_userName->setText( map["clinicUserName"].toString() );
   }
}

QMap<QString, QVariant> w_DatabaseConnectionWidget::getDbControllerProperties()
{
    QMap<QString, QVariant> map = this->GetWatchedObjectProperties("databaseController", "basicOnly");

    return map;
}

QMap<QString, QVariant> w_DatabaseConnectionWidget::getDbConnectionProperties()
{
    QMap<QString, QVariant> map;

    if(this->dbName == "Authorization")
    {
        map.insert( "authDriver", ui->f_driverName->currentText() );
        map.insert( "authAddress", ui->f_hostName->text() );
        map.insert( "authDbName", ui->f_dbName->text() );
        map.insert( "authPort", ui->f_port->text() );
        map.insert( "authUserName", ui->f_userName->text() );
        map.insert( "authPassword", ui->f_password->text() );
    }
    else if (this->dbName == "Clinic")
    {
        map.insert( "clinicDriver", ui->f_driverName->currentText() );
        map.insert( "clinicAddress", ui->f_hostName->text() );
        map.insert( "clinicDbName", ui->f_dbName->text() );
        map.insert( "clinicPort", ui->f_port->text() );
        map.insert( "clinicUserName", ui->f_userName->text() );
        map.insert( "clinicPassword", ui->f_password->text() );
    }


    return map;
}

void w_DatabaseConnectionWidget::SetName(QString name)
{
    this->dbName = name;
}

void w_DatabaseConnectionWidget::on_testConnectionButton_clicked()
{
    c_MyDatabaseConnection * connection = new c_MyDatabaseConnection(ui->f_hostName->text(), ui->f_port->text().toInt(),
                                                     ui->f_driverName->currentText(), ui->f_dbName->text(),
                                                     ui->f_userName->text(), ui->f_password->text());

    ui->testConnectionButton->setEnabled(false);

    QTimer * timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(RefreshButtonOTimeout()));

    if(connection->TestConnection(5000))
    {
        ui->testConnectionButton->setText("Udało się nawiązać połączenie");
        ui->testConnectionButton->setStyleSheet("border-color: green;"
                                                "color: green;");
        timer->start(1500);
    }
    else
    {
        ui->testConnectionButton->setText("Nie udało się nawiązać połączenia");
        ui->testConnectionButton->setStyleSheet("border-color: red;"
                                                "color: red;");
        timer->start(1500);
    }

    delete connection;
}

void w_DatabaseConnectionWidget::RefreshButtonOTimeout()
{
    ui->testConnectionButton->setStyleSheet("border-color: #000000;"
                                            "color: #000000;");
    ui->testConnectionButton->setText("Test");
    ui->testConnectionButton->setEnabled(true);
    delete sender();
}

void w_DatabaseConnectionWidget::TestStateChanged(QAbstractSocket::SocketState state)
{
    if (state == QAbstractSocket::ConnectingState)
    {

        ui->testConnectionButton->setText("Nawiązuję połączenie");
        ui->testConnectionButton->setStyleSheet("border-color: orange;"
                                                "color: orange;");

    }
}
