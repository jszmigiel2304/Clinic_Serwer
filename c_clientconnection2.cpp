#include "c_clientconnection2.h"

c_ClientConnection2::c_ClientConnection2(qintptr ID, QObject *parent)
    : QObject{parent}
{    
    this->socketDescriptor = ID;

    this->socket = new QTcpSocket(this);

    this->executive = new c_actionExecutive(this);

    if(!this->socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(this->socket->error());

        QString log = QString("c_ClientConnection::run() \n"
                              "error(this->socket->error())");
        emit newLog(log);

        return;
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    connect(this, SIGNAL(dataRead(quint64, QByteArray, qintptr)), this->executive, SLOT(dataReceived(quint64, QByteArray, qintptr)), Qt::DirectConnection);
    connect(this->executive, SIGNAL(reply(quint64, QByteArray)), this, SLOT(sendDataToClient(quint64, QByteArray)), Qt::DirectConnection);

    socket->open(QIODeviceBase::ReadWrite);


}

c_ClientConnection2::~c_ClientConnection2()
{
    if(socket != nullptr && socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->disconnectFromHost();
        socket->deleteLater();
    }

    executive->deleteLater();

}



void c_ClientConnection2::sendDataToClient(quint64 size, QByteArray data)
{
        QDataStream socketStream(socket);
        socketStream.setVersion(QDataStream::Qt_6_0);


        socketStream.startTransaction();

        socketStream << size << data;


        if (!socketStream.commitTransaction())
            return;     // wait for more data

        emit newLogToFile(QString("c_ClientConnection::reply(quint32 size, QByteArray data)"), QString("%1").arg(socket->socketDescriptor()), data);


        emit newLog(QString("/nc_ClientConnection::reply(quint32 size, QByteArray data) - data sent"));


}

c_actionExecutive *c_ClientConnection2::getExecutive() const
{
    return executive;
}

void c_ClientConnection2::setExecutive(c_actionExecutive *newExecutive)
{
    executive = newExecutive;
}

qintptr c_ClientConnection2::getSocketDescriptor() const
{
    return socketDescriptor;
}

void c_ClientConnection2::setSocketDescriptor(qintptr newSocketDescriptor)
{
    socketDescriptor = newSocketDescriptor;
}

QTcpSocket *c_ClientConnection2::getSocket() const
{
    return socket;
}

void c_ClientConnection2::setSocket(QTcpSocket *newSocket)
{
    socket = newSocket;
}

void c_ClientConnection2::readyRead()
{
    QString log("Start read :: c_ClientConnection::readyRead()");
    emit newLog(log);

    QDataStream socket_dataStream(socket);
    socket_dataStream.setVersion(QDataStream::Qt_6_0);


    QByteArray data;
    quint64 data_size;

    socket_dataStream.startTransaction();

    socket_dataStream >> data_size >> data; // try to read packet atomically

    if (!socket_dataStream.commitTransaction())
        return;     // wait for more data



    emit newLogToFile(QString("c_ClientConnection::readyRead"), QString("%1").arg(socketDescriptor), data);
    log = "End read :: c_ClientConnection::readyRead()";
    emit newLog(log);

    emit dataRead(data_size, data, this->getSocketDescriptor());
}

void c_ClientConnection2::disconnected()
{
        emit connectionFinished(this);
}

