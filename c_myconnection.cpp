#include "c_myconnection.h"

c_MyConnection::c_MyConnection(QObject *parent) : QObject(parent)
{

}

c_MyConnection::c_MyConnection(QString hostAddress, quint16 port)
{
    this->HostAddress = hostAddress;
    this->Port = port;
}

QString c_MyConnection::getHostAddress() const
{
    return HostAddress;
}

void c_MyConnection::setHostAddress(const QString &value)
{
    HostAddress = value;
}

quint16 c_MyConnection::getPort() const
{
    return Port;
}

void c_MyConnection::setPort(const quint16 &value)
{
    Port = value;
}

bool c_MyConnection::TestConnection(int time)
{
    QTcpSocket * socket  = new QTcpSocket(this);
    bool testResult;

    socket->connectToHost(this->HostAddress, this->Port);

    if(socket->waitForConnected(time))
    {
        testResult = true;
    }
    else
    {
        testResult = false;
    }


    socket->close();
    delete socket;

    return testResult;
}
