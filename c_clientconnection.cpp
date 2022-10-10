#include "c_clientconnection.h"

c_ClientConnection::c_ClientConnection(qintptr ID, QObject *parent)
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
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::QueuedConnection);

    connect( this, SIGNAL(sendDataToClientSignal(packet)), this, SLOT(sendDataToClient(packet)), Qt::DirectConnection );

    connect(this, SIGNAL(dataParsed(processedThreadData)), this->executive, SLOT(dataReceived(processedThreadData)));
    connect(this, SIGNAL(dataRead(quint64, QByteArray, qintptr)), this, SLOT(parseReceivedPacket(quint64, QByteArray, qintptr)));
//    connect(this->executive, SIGNAL(reply(quint64, QByteArray)), this, SLOT(sendDataToClient(quint64, QByteArray)), Qt::DirectConnection);
    connect(this->executive, SIGNAL(replyReady(QByteArray, QByteArray)), this, SLOT(replyReceived(QByteArray, QByteArray)), Qt::DirectConnection);


    waitForFinishProcessingTimer = new QTimer();
    connect(waitForFinishProcessingTimer, SIGNAL(timeout()), this, SLOT(waitForFinishProcessingTimerTimeOut()));

    socket->open(QIODeviceBase::ReadWrite);


}

c_ClientConnection::~c_ClientConnection()
{
    waitForFinishProcessingTimer->deleteLater();

    if(socket != nullptr && socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->disconnectFromHost();
        socket->deleteLater();
    }


    executive->deleteLater();

}

void c_ClientConnection::removeFromprocessedPackets(QByteArray md5Hash)
{
    for(int i = 0; i < processedPackets.length(); i++) {
        if( processedPackets[i].md5_hash == md5Hash ) {
            processedPackets.removeAt(i);
            break;

        }
    }
}

bool c_ClientConnection::isProcessed(QByteArray md5Hash)
{
    bool processed = false;

    for(int i = 0; i < processedPackets.length(); i++) {
        if( processedPackets[i].md5_hash == md5Hash ) {
            processed = true;
            break;
        }
    }

    return processed;
}



//void c_ClientConnection::sendDataToClient(quint64 size, QByteArray data)
//{
//        QDataStream socketStream(socket);
//        socketStream.setVersion(QDataStream::Qt_6_0);


//        socketStream.startTransaction();

//        socketStream << size << data;


//        if (!socketStream.commitTransaction())
//            return;     // wait for more data

//        emit newLogToFile(QString("c_ClientConnection::reply(quint32 size, QByteArray data)"), QString("%1").arg(socket->socketDescriptor()), data);
//}

void c_ClientConnection::replyReceived(QByteArray processedRequestMd5Hash, QByteArray json)
{
    c_myParser parser;

    packet packet;
    packet.md5_hash = parser.getJsonMD5Hash( QJsonDocument::fromJson(json) );
    //packet.packet_to_send = json;

    QDataStream ds2(&packet.packet_to_send, QIODevice::ReadWrite);
    ds2.setVersion(QDataStream::Qt_6_0);

//    ds2 << static_cast<quint8>(thread_dest) << thread_id << static_cast<quint8>(req_type) << static_cast<quint32>(type_flag) << json.toJson();
    ds2 << packet.md5_hash.toHex() << json;


    sendDataToClient(packet);
    removeFromprocessedPackets(processedRequestMd5Hash);
}

bool c_ClientConnection::replyRequired(threadData *request)
{
    bool replyRequired = true;

    return replyRequired;
}

void c_ClientConnection::waitForFinishProcessingTimerTimeOut()
{
    QString log = QString("Kończę pracę, proszę czekać.... \n");
    emit newLog(log);
}

void c_ClientConnection::parseReceivedPacket(quint64 size, QByteArray data, qintptr socketDescriptor)
{
    c_myParser parser;

    QPair<QByteArray, QByteArray> pair = parser.ParseReceivedPacket(size, data);
    threadData jsonPacketData = parser.ParseJsonPacket( pair.second, socketDescriptor );

    processedThreadData processedRequest;
    processedRequest.md5Hash = pair.first;
    processedRequest.socketDescriptor = jsonPacketData.socketDescriptor;
    processedRequest.content = jsonPacketData.content;
    processedRequest.thread_id = jsonPacketData.thread_id;
    processedRequest.thread_dest = jsonPacketData.thread_dest;
    processedRequest.req_type = jsonPacketData.req_type;
    processedRequest.type_flag = jsonPacketData.type_flag;
    processedRequest.data = jsonPacketData.data;

    processedPacket processedPacket;
    processedPacket.md5_hash = pair.first;
    processedPacket.data = jsonPacketData;
    processedPacket.wait_for_reply = replyRequired(&jsonPacketData);

    QByteArray hex = pair.first.toHex();

    /*----------------------------------------------------*/

    packet packet;
    QMap<QString, QVariant> packetInfo;
    packetInfo["thread_dest"] = static_cast<qint8>(CLINIC_CONNECTION_CONTROLLER);
    packetInfo["thread_id"] = processedRequest.thread_id;
    packetInfo["req_type"] = static_cast<qint8>(RECEIVE_CONFIRMATION);
    packetInfo["type_flag"] = 0x00000001;
    packetInfo["content"] = static_cast<qint32>(PACKET_RECEIVE_CONFIRMATION);

    QJsonDocument receiveConfirmation = parser.prepareReceiveConfirmationJSON(packetInfo, processedRequest.md5Hash);

    packet.wait_for_reply = false;
    packet.md5_hash = parser.getJsonMD5Hash( receiveConfirmation );
    //packet.packet_to_send = receiveConfirmation.toJson();

    QDataStream ds2(&packet.packet_to_send, QIODevice::ReadWrite);
    ds2.setVersion(QDataStream::Qt_6_0);

//    ds2 << static_cast<quint8>(thread_dest) << thread_id << static_cast<quint8>(req_type) << static_cast<quint32>(type_flag) << json.toJson();
    ds2 << packet.md5_hash.toHex() << receiveConfirmation.toJson();

    sendDataToClient(packet);

    /*----------------------------------------------------------------*/

    if(!isProcessed(processedPacket.md5_hash)) {
        if(processedPacket.wait_for_reply)
            processedPackets.append(processedPacket);

        emit dataParsed(processedRequest);
    }
}

c_actionExecutive *c_ClientConnection::getExecutive() const
{
    return executive;
}

void c_ClientConnection::setExecutive(c_actionExecutive *newExecutive)
{
    executive = newExecutive;
}

qintptr c_ClientConnection::getSocketDescriptor() const
{
    return socketDescriptor;
}

void c_ClientConnection::setSocketDescriptor(qintptr newSocketDescriptor)
{
    socketDescriptor = newSocketDescriptor;
}

QTcpSocket *c_ClientConnection::getSocket() const
{
    return socket;
}

void c_ClientConnection::setSocket(QTcpSocket *newSocket)
{
    socket = newSocket;
}

void c_ClientConnection::readyRead()
{
    /*----------------READ 1 ------------------------------------*/
//    QDataStream socket_dataStream(socket);
//    socket_dataStream.setVersion(QDataStream::Qt_6_0);


//    QByteArray data;
//    quint64 data_size;

//    socket_dataStream.startTransaction();

//    socket_dataStream >> data_size >> data; // try to read packet atomically

//    if (!socket_dataStream.commitTransaction())
//        return;     // wait for more data



//    emit newLogToFile(QString("c_ClientConnection::readyRead"), QString("%1").arg(socketDescriptor), data);
//    QString log = QString("%1 has been read. \n").arg(data_size);
//    emit newLog(log);

//    emit dataRead(data_size, data, this->getSocketDescriptor());

    /*----------------READ 1 ------------------------------------*/


    /*----------------READ 2 ------------------------------------*/

    QByteArray myPack;

    while(socket->canReadLine()) {
        QByteArray line = QByteArray( socket->readLine() );
        if( QString::fromUtf8(line) == QString("PACKET_BEGINNING\n") ) {
            myPack.clear();
        } else if( QString::fromUtf8(line) == QString("PACKET_END\n") ) {
            emit newLogToFile(QString("c_ClientConnection::readyRead"), QString("%1").arg(socketDescriptor), myPack);
            QString log = QString("%1 has been read. \n").arg(myPack.size());
            emit newLog(log);

            emit dataRead(myPack.size(), myPack, this->getSocketDescriptor());
        } else {
            myPack.append(line);
        }
    }

    /*----------------READ 2 ------------------------------------*/
}

void c_ClientConnection::disconnected()
{
    while(executive->getProcessingMode()) {
        waitForFinishProcessingTimer->start(1000);
    }

    emit connectionFinished(this);
}

void c_ClientConnection::sendDataToClient(packet packet)
{
    /*----------------------SEND 1--------------------------*/
//    QDataStream socketStream(socket);
//    socketStream.setVersion(QDataStream::Qt_6_0);


//    socketStream.startTransaction();

//    socketStream << static_cast<quint64>(packet.packet_to_send.size()) << packet.packet_to_send;

//    if (!socketStream.commitTransaction())
//        return;     // wait for more data

//    emit newLogToFile(QString("c_ClientConnection::reply(quint32 size, QByteArray data)"), QString("%1").arg(socket->socketDescriptor()), packet.packet_to_send);
//    QString log = QString("%1 has been written. \n").arg(packet.packet_to_send.size());
//    emit newLog(log);
    /*----------------------SEND 1--------------------------*/


    /*----------------------SEND 2--------------------------*/

        socket->write( QString("PACKET_BEGINNING\n").toUtf8() );
        socket->write( packet.packet_to_send );
        socket->write( QString("PACKET_END\n").toUtf8() );

        emit newLogToFile(QString("c_ClientConnection::reply(quint32 size, QByteArray data)"), QString("%1").arg(socket->socketDescriptor()), packet.packet_to_send);
        QString log = QString("%1 has been written. \n").arg(packet.packet_to_send.size());
    /*----------------------SEND 2--------------------------*/
}

