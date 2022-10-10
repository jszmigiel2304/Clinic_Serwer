#include "c_mydatabaseconnection.h"

c_MyDatabaseConnection::c_MyDatabaseConnection()
{

}

c_MyDatabaseConnection::c_MyDatabaseConnection(QString hostAddress, quint16 port, QString driver, QString databaseName, QString userName, QString password, QObject * parent)
    : c_MyConnection(parent)
{
    this->HostAddress = hostAddress;
    this->Port = port;
    this->Driver = driver;
    this->DatabaseName = databaseName;
    this->UserName = userName;
    this->Password = password;
}

QString c_MyDatabaseConnection::getDriver() const
{
    return Driver;
}

void c_MyDatabaseConnection::setDriver(const QString &value)
{
    Driver = value;
}

QString c_MyDatabaseConnection::getDatabaseName() const
{
    return DatabaseName;
}

void c_MyDatabaseConnection::setDatabaseName(const QString &value)
{
    DatabaseName = value;
}

QString c_MyDatabaseConnection::getUserName() const
{
    return UserName;
}

void c_MyDatabaseConnection::setUserName(const QString &value)
{
    UserName = value;
}

QString c_MyDatabaseConnection::getPassword() const
{
    return Password;
}

void c_MyDatabaseConnection::setPassword(const QString &value)
{
    Password = value;
}

bool c_MyDatabaseConnection::TestConnection(int time)
{
    QTcpSocket socket(this);
    bool testResult;

    socket.connectToHost(this->HostAddress, this->Port);

    if(socket.waitForConnected(time))
    {
        testResult = true;
    }
    else
    {
        testResult = false;
    }


    socket.close();

    if(!testResult)
        return testResult;


    if(!QSqlDatabase::isDriverAvailable(this->Driver))
        return false;

    QSqlDatabase::database(this->DatabaseName).close();
    QSqlDatabase::removeDatabase(this->DatabaseName);
    QSqlDatabase::addDatabase(this->Driver, this->DatabaseName);

    QSqlDatabase::database(this->DatabaseName).setDatabaseName(this->DatabaseName);
    QSqlDatabase::database(this->DatabaseName).setHostName(this->HostAddress);
    QSqlDatabase::database(this->DatabaseName).setUserName(this->UserName);
    QSqlDatabase::database(this->DatabaseName).setPassword(this->Password);
    QSqlDatabase::database(this->DatabaseName).setPort(this->Port);


    if(!QSqlDatabase::database((this->DatabaseName)).isOpen())
    {
        if(this->Driver == SQLITE_VERSION_2_DRIVER || SQLITE_VERSION_3_DRIVER)
            QSqlDatabase::database((this->DatabaseName)).setConnectOptions("QSQLITE_BUSY_TIMEOUT=5000");
          else if(this->Driver == POSTGRESQL_DRIVER)
            QSqlDatabase::database((this->DatabaseName)).setConnectOptions("connect_timeout=5000");
          else if(this->Driver == MYSQL_DRIVER)
            QSqlDatabase::database((this->DatabaseName)).setConnectOptions("MYSQL_OPT_CONNECT_TIMEOUT=5000");
          else if(this->Driver == ODBC_MSSQL_DRIVER)
            QSqlDatabase::database((this->DatabaseName)).setConnectOptions("SQL_ATTR_CONNECTION_TIMEOUT=5000");
          else
            QSqlDatabase::database((this->DatabaseName)).setConnectOptions();
    }



    QSqlDatabase::database(this->DatabaseName).open();

    if(QSqlDatabase::database(this->DatabaseName).isOpen())
        testResult = true;
    else
        testResult = false;


    QSqlDatabase::database(this->DatabaseName).close();
    QSqlDatabase::removeDatabase(this->DatabaseName);

    return testResult;
}
