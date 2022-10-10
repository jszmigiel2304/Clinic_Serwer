#include "c_clinictcpserver.h"

c_ClinicTcpServer::c_ClinicTcpServer()
{

}

c_ClinicTcpServer::c_ClinicTcpServer(QMap<QString, QVariant> settings, QObject *parent) :
    QTcpServer(parent)
{
    this->iterfaceName = settings["interfaceName"].toString();
    this->port = quint16(settings["port"].toInt());

        if(settings["interfaceName"].toString() == "any")
            this->address = QHostAddress::Any;
        else
        {
            QNetworkInterface interface;
            foreach (QNetworkInterface i, QNetworkInterface::allInterfaces()) {
                if (i.name() == settings["interfaceName"].toString())
                    interface = i;
            }

            this->address = ((interface.addressEntries())[interface.addressEntries().length() - 1]).ip();
        }

}

c_ClinicTcpServer::~c_ClinicTcpServer()
{

}

QMap<QString, QVariant> c_ClinicTcpServer::ShareProperties(QString sharedData)
{
    QMap<QString, QVariant> map;

    if(sharedData == "all" || sharedData == "basicOnly")
    {
        map.insert("port", this->port);
        map.insert("interfaceName", this->iterfaceName);
        map.insert("address", this->address.toString());
        map.insert("isListening", this->isListening());
        map.insert("hostsNumber", this->hostsList.count());
    }

    if(sharedData == "all" || sharedData == "hostsOnly")
    {
        foreach (c_ClientConnection * host, this->hostsList) {
            qintptr soc =  host->getSocket()->socketDescriptor();
            map.insert(host->getSocket()->peerAddress().toString(), soc );
        }
    }

    return map;
}

QString c_ClinicTcpServer::ShareMessege()
{
    return this->status;
}

void c_ClinicTcpServer::runServer()
{
    this->status = "Sprawdzam połączenie z bazą danych:\n";
    this->status += dbContr->getAuthDbName();
    this->status += "\n";
    this->status += dbContr->getAuthDbHostName();
    this->status += ":";
    this->status += QString::number(dbContr->getAuthDbPort());

    emit this->MessageChanged(this->status, -1);

    c_MyDatabaseConnection connection(dbContr->getAuthDbHostName(), dbContr->getAuthDbPort(), dbContr->getAuthDriver(), dbContr->getAuthDbName(), dbContr->getAuthDbUserName(), dbContr->getAuthPassword());


    if(connection.TestConnection(5000))
    {
        dbContr->AddDatabase("Authorization");
        dbContr->SetUpDatabase("Authorization");
        this->status = "   [ Authorization Database Connection ] Poprawnie skonfigurowane";

        QString log = QString("[ Authorization Database Connection ] Poprawnie skonfigurowane");
        emit newLog(log);
    }
    else
    {
        this->status = "   [ Authorization Database Connection ] Błąd";

        QString log = QString("[ Authorization Database Connection ] Błąd");
        emit newLog(log);
    }

    emit this->MessageChanged(this->status, 1000);


    this->status = "Sprawdzam połączenie z bazą danych:\n";
    this->status += dbContr->getClinicDbName();
    this->status += "\n";
    this->status += dbContr->getClinicDbHostName();
    this->status += ":";
    this->status += QString::number(dbContr->getClinicDbPort());

    emit this->MessageChanged(this->status, -1);

    connection.setHostAddress(dbContr->getClinicDbHostName());
    connection.setPort(dbContr->getClinicDbPort());
    connection.setDriver(dbContr->getClinicDriver());
    connection.setDatabaseName(dbContr->getClinicDbName());
    connection.setUserName(dbContr->getClinicDbUserName());
    connection.setPassword(dbContr->getClinicPassword());

    if(connection.TestConnection(5000))
    {
        dbContr->AddDatabase("Clinic");
        dbContr->SetUpDatabase("Clinic");
        this->status = "   [ Clinic Database Connection ] Poprawnie skonfigurowane";

        QString log = QString("[ Clinic Database Connection ] Poprawnie skonfigurowane");
        emit newLog(log);
    }
    else
    {
        this->status = "   [ Clinic Database Connection ] Błąd";

        QString log = QString("[ Clinic Database Connection ] Błąd");
        emit newLog(log);
    }


    emit this->MessageChanged(this->status, 1000);

    this->status = "Uruchamiam serwer.";

    emit this->MessageChanged(this->status, -1);


    this->listen(this->address, this->port);



    if(!this->isListening())
    {
        this->status = "[ Server TCP ]   Nie udało się uruchomić";
        emit this->MessageChanged(this->status, 2000);

        QString log = QString("Nie udało się uruchomić serwera.");
        emit newLog(log);
    }
     else
    {
        this->status = "[ Server TCP ] Uruchomiony";
        emit this->MessageChanged(this->status, 2000);

        QString log = QString("Uruchomiono serwer TCP/IP");
        emit newLog(log);
    }


    emit this->PropertiesChanged();
    emit this->dbContr->PropertiesChanged();

}

void c_ClinicTcpServer::stopServer()
{
    this->close();
    this->removeClients();
    this->hostsList.clear();
    this->dbContr->RemoveAllDatabases();
    emit this->PropertiesChanged();
    emit this->dbContr->PropertiesChanged();

    this->status = "[Server TCP] Zatrzymano";

    QString log = QString("[Server TCP] Zatrzymano");
    logs->addLog(log);
    emit this->MessageChanged(this->status, 2000);

}

void c_ClinicTcpServer::startServer()
{
        if(!this->listen(this->address, this->port))
        {
        }
        else
        {
        }

}

void c_ClinicTcpServer::UpdateProperties(QMap<QString, QVariant> map)
{
    this->port = quint16(map["port"].toInt());
    this->iterfaceName = map["interfaceName"].toString();

    if(map["interfaceName"].toString() == "any")
        this->address = QHostAddress::Any;
    else
    {
        QNetworkInterface interface;
        foreach (QNetworkInterface i, QNetworkInterface::allInterfaces()) {
            if (i.name() == map["interfaceName"].toString())
                interface = i;
        }

        this->address = ((interface.addressEntries())[interface.addressEntries().length() - 1]).ip();
    }

    emit this->PropertiesChanged();
}


c_MySqlDatabaseController *c_ClinicTcpServer::getDbContr()
{
    return dbContr;
}

void c_ClinicTcpServer::createDbContr(QMap<QString, QVariant> authDbSettings, QMap<QString, QVariant> clinicDbSettings)
{

    this->dbContr = new c_MySqlDatabaseController(authDbSettings, clinicDbSettings);
}


QList<c_ClientConnection *> c_ClinicTcpServer::getHostsList() const
{
    return hostsList;
}

void c_ClinicTcpServer::incomingConnection(qintptr socketDescriptor)
{
    c_ClientConnection *connection = new c_ClientConnection(socketDescriptor);

    connect(connection, SIGNAL(newLog(QString)), logs, SLOT(addLog(QString)));
    connect(connection, SIGNAL(newLogToFile(QString,QString,QByteArray)), logsContr, SLOT(saveLogToFile(QString,QString,QByteArray)));


    connect(connection->getExecutive(), SIGNAL(newLog(QString)), logs, SLOT(addLog(QString)));
    connect(connection->getExecutive(), SIGNAL(newLogToFile(QString,QString,QByteArray)), logsContr, SLOT(saveLogToFile(QString,QString,QByteArray)));

    connection->getExecutive()->setDataBasesCtrlr(this->getDbContr());
    //connect(connection->getExecutive(), SIGNAL(exeDataBaseQuery(QString, QString, QList<QMap<QString,QVariant>> *, QStringList *)), this->getDbContr(), SLOT(exe(QString, QString, QList<QMap<QString,QVariant>> *, QStringList *)));

    connect(connection, SIGNAL(newClient(c_ClientConnection*)), this, SLOT(newClient(c_ClientConnection*)));
    connect(connection, SIGNAL(connectionFinished(c_ClientConnection*)), this, SLOT(removeClient2(c_ClientConnection*)));




    QString log = QString("incomingConnection(qintptr socketDescriptor) \n"
                          "connection->start(); \n");


    emit connection->newClient(connection);
    emit newLog(log);
}

void c_ClinicTcpServer::setDbContr(c_MySqlDatabaseController *value)
{
    dbContr = value;
}

void c_ClinicTcpServer::newClient(c_ClientConnection *connection)
{
    this->hostsList.push_back(connection);
    QString log = QString("c_ClinicTcpServer::newClient(c_ClientConnection *connection)  \n"
                          "dodano połączenie \n");
    emit newLog(log);
    emit this->PropertiesChanged();
}

void c_ClinicTcpServer::removeClient(qintptr id)
{
    c_ClientConnection * temp;
    for(int i = 0; i < this->hostsList.size();)
    {
        if(hostsList[i]->getSocketDescriptor() == id)
        {
            temp = hostsList.takeAt(i);
            delete temp;
            break;
        }
    }


    emit this->PropertiesChanged();
}

void c_ClinicTcpServer::removeClient2(c_ClientConnection * connection)
{
    hostsList.removeAll(connection);
    hostsList.squeeze();

    emit this->PropertiesChanged();
}

void c_ClinicTcpServer::removeClient(quint32 index)
{
    c_ClientConnection * temp;
    temp = hostsList.takeAt(index);
    delete temp;
}

c_LogsController *c_ClinicTcpServer::getLogsContr() const
{
    return logsContr;
}

void c_ClinicTcpServer::setLogsContr(c_LogsController *newLogsContr)
{
    logsContr = newLogsContr;
}

w_logsWindow *c_ClinicTcpServer::getLogs() const
{
    return logs;
}

void c_ClinicTcpServer::setLogs(w_logsWindow *newLogs)
{
    logs = newLogs;
    connect(this, SIGNAL(newLog(QString)), logs, SLOT(addLog(QString)));
}


void c_ClinicTcpServer::removeClients()
{
    c_ClientConnection * temp;
    for(int i = 0; i < this->hostsList.size();)
    {
        temp = hostsList.takeAt(0);
        delete temp;
    }

    emit this->PropertiesChanged();
}

