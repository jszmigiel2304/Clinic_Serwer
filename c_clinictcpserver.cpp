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

        logs = w_logsWindow::Instance();

}

c_ClinicTcpServer::~c_ClinicTcpServer()
{
    parser->deleteLater();
    executive->deleteLater();
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
    QTcpSocket * socket = new QTcpSocket();

    this->status = "Sprawdzam połączenie z bazą danych:\n";
    this->status += dbContr->getAuthDbName();
    this->status += "\n";
    this->status += dbContr->getAuthDbHostName();
    this->status += ":";
    this->status += QString::number(dbContr->getAuthDbPort());

    emit this->MessageChanged(this->status, -1);

    c_MyDatabaseConnection * connection = new c_MyDatabaseConnection(dbContr->getAuthDbHostName(), dbContr->getAuthDbPort(),
                                                                     dbContr->getAuthDriver(), dbContr->getAuthDbName(),
                                                                     dbContr->getAuthDbUserName(), dbContr->getAuthPassword());

    if(connection->TestConnection(5000))
    {
        dbContr->AddDatabase("Authorization");
        dbContr->SetUpDatabase("Authorization");
        this->status = "   [ Authorization Database Connection ] Poprawnie skonfigurowane";

        QString log = QString("[ Authorization Database Connection ] Poprawnie skonfigurowane");
        logs->addLog(log);
    }
    else
    {
        this->status = "   [ Authorization Database Connection ] Błąd";

        QString log = QString("[ Authorization Database Connection ] Błąd");
        logs->addLog(log);
    }

    emit this->MessageChanged(this->status, 1000);


    this->status = "Sprawdzam połączenie z bazą danych:\n";
    this->status += dbContr->getClinicDbName();
    this->status += "\n";
    this->status += dbContr->getClinicDbHostName();
    this->status += ":";
    this->status += QString::number(dbContr->getClinicDbPort());

    emit this->MessageChanged(this->status, -1);

    connection->setHostAddress(dbContr->getClinicDbHostName());
    connection->setPort(dbContr->getClinicDbPort());
    connection->setDriver(dbContr->getClinicDriver());
    connection->setDatabaseName(dbContr->getClinicDbName());
    connection->setUserName(dbContr->getClinicDbUserName());
    connection->setPassword(dbContr->getClinicPassword());

    if(connection->TestConnection(5000))
    {
        dbContr->AddDatabase("Clinic");
        dbContr->SetUpDatabase("Clinic");
        socket->disconnectFromHost();
        this->status = "   [ Clinic Database Connection ] Poprawnie skonfigurowane";

        QString log = QString("[ Clinic Database Connection ] Poprawnie skonfigurowane");
        logs->addLog(log);
    }
    else
    {
        this->status = "   [ Clinic Database Connection ] Błąd";

        QString log = QString("[ Clinic Database Connection ] Błąd");
        logs->addLog(log);
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
        logs->addLog(log);
    }
     else
    {
        this->parser = new c_myParser(this);
        this->parser->setLogs(logs);
        this->executive = new c_actionExecutive(this);
        this->executive->setLogs(logs);

        this->status = "[ Server TCP ] Uruchomiony";
        emit this->MessageChanged(this->status, 2000);

        QString log = QString("Uruchomiono serwer TCP/IP");
        logs->addLog(log);
    }

    delete socket;    

    emit this->PropertiesChanged();
    emit this->dbContr->PropertiesChanged();


}

void c_ClinicTcpServer::stopServer()
{
    this->removeClients();
    this->close();
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

void c_ClinicTcpServer::dataRead(quint32 size, QByteArray data)
{
    threadData received_data = parser->Parse(size, data);

    executive->newDataReceivedFromClient(received_data);

        QString log = QString("Data received (Total Data in buffer %1) \n"
                              "connection->start(); \n").arg(executive->getReceivedDataFromClients().size());
        //logs->clearLogs();
        logs->addLog(log);
}

c_MySqlDatabaseController *c_ClinicTcpServer::getDbContr() const
{
    return dbContr;
}

c_actionExecutive *c_ClinicTcpServer::getExecutive() const
{
    return executive;
}

void c_ClinicTcpServer::setExecutive(c_actionExecutive *value)
{
    executive = value;
}

c_myParser *c_ClinicTcpServer::getParser() const
{
    return parser;
}

void c_ClinicTcpServer::setParser(c_myParser *value)
{
    parser = value;
}

QList<c_ClientConnection *> c_ClinicTcpServer::getHostsList() const
{
    return hostsList;
}

void c_ClinicTcpServer::incomingConnection(qintptr socketDescriptor)
{
    c_ClientConnection *connection = new c_ClientConnection(socketDescriptor, this);

    connect(connection, SIGNAL(dataRead(quint32,QByteArray)), this, SLOT(dataRead(quint32,QByteArray)));
    connect(connection, SIGNAL(finished()), connection, SLOT(deleteLater()));

//    QString log = QString("incomingConnection(qintptr socketDescriptor) \n"
//                          "connection->start(); \n");
//    logs->addLog(log);

    connection->start();
}

void c_ClinicTcpServer::setDbContr(c_MySqlDatabaseController *value)
{
    dbContr = value;
}

void c_ClinicTcpServer::newClient(c_ClientConnection *connection)
{
    this->hostsList.push_back(connection);
    emit this->PropertiesChanged();
}

void c_ClinicTcpServer::removeClient(qintptr id)
{
    for(int i = this->hostsList.count() - 1; i >= 0; i--)
    {
        if(hostsList[i]->getSocketDescriptor() == id)
        {
            hostsList.removeAt(i);
            break;
        }
        //this->hostsList[i]->disconnected();
    }

//    QMutableListIterator<c_ClientConnection *> iterator(this->hostsList);

//    while(iterator.hasNext())
//    {
//        if(iterator.next()->getSocketDescriptor() == id)
//        {
//            iterator.remove();
//            break;
//        }
//    }

    emit this->PropertiesChanged();
}

//void c_ClinicTcpServer::removeClient(c_ClientConnection *connection)
//{
//}

void c_ClinicTcpServer::removeClients()
{
    for(int i = this->hostsList.count() - 1; i >= 0; i--)
    {
        hostsList[i]->disconnected();
    }

    hostsList.clear();
}

