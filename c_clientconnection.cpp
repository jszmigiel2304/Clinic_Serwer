#include "c_clientconnection.h"
#include "c_clinictcpserver.h"


c_ClientConnection::c_ClientConnection(qintptr ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
    this->logs = w_logsWindow::Instance();
}

c_ClientConnection::~c_ClientConnection()
{
}

void c_ClientConnection::run()
{

    this->socket = new QSslSocket();

    if(!this->socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(this->socket->error());

        QString log = QString("c_ClientConnection::run() \n"
                              "error(this->socket->error())");
        w_logsWindow::Instance()->addLog(log);

        return;
    }


    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket,SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::DirectConnection);


    (dynamic_cast<c_ClinicTcpServer *>(parent()))->newClient(this);

    socket->open(QIODevice::ReadWrite);

    exec();

}

void c_ClientConnection::reply(QByteArray answer)
{
    this->socket->write(answer);
}

void c_ClientConnection::readyRead()
{

    logs->addSeparator();
    QString log("Start read :: c_ClientConnection::readyRead()");
    logs->addLog(log);


    QDataStream socket_dataStream(socket);
    socket_dataStream.setVersion(QDataStream::Qt_6_0);


    QByteArray data;
    quint32 data_size;

    socket_dataStream.startTransaction();

    socket_dataStream >> data_size >> data; // try to read packet atomically

    if (!socket_dataStream.commitTransaction())
        return;     // wait for more data



    c_LogsController::Instance()->saveLogToFile(QString("c_ClientConnection::readyRead"), QString("%1").arg(socketDescriptor), data);


    log = "End read :: c_ClientConnection::readyRead()";
    logs->addLog(log);
    logs->addSeparator();


    emit dataRead(data_size, data);

    //!!!!!!!!!!! odpowiedz serwera

//    QByteArray reply_data;
//    quint32 reply_data_size;
//    QString msg("dane logowania otrzymane");

//    QDataStream reply_data_msg(&reply_data, QIODevice::ReadWrite);
//    reply_data_msg.setVersion(QDataStream::Qt_6_0);

//    reply_data_msg << msg;
//    socket_dataStream << static_cast<quint32>(reply_data_size);
//    socket_dataStream << reply_data;

}

void c_ClientConnection::disconnected()
{
//    QString log = QString("c_ClientConnection::disconnected() \n");
//    w_logsWindow::Instance()->addLog(log);

    (dynamic_cast<c_ClinicTcpServer *>(parent()))->removeClient(this->socketDescriptor);
    socket->deleteLater();
    exit(0);
}

qintptr c_ClientConnection::getSocketDescriptor() const
{
    return socketDescriptor;
}

void c_ClientConnection::setSocketDescriptor(const qintptr &value)
{
    socketDescriptor = value;
}



QSslSocket *c_ClientConnection::getSocket() const
{
    return socket;
}

void c_ClientConnection::setSocket(QSslSocket *value)
{
    socket = value;
}
