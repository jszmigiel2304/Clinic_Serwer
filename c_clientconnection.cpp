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

        return;
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::QueuedConnection);

    connect( this, SIGNAL(sendDataToClientSignal(myStructures::packet)), this, SLOT(sendDataToClient(myStructures::packet)), Qt::DirectConnection );

    connect(this, SIGNAL(dataParsed(myStructures::processedThreadData)), this->executive, SLOT(dataReceived(myStructures::processedThreadData)));
    connect(this, SIGNAL(dataRead(quint64, QByteArray, qintptr)), this, SLOT(parseReceivedPacket(quint64, QByteArray, qintptr)));
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

void c_ClientConnection::replyReceived(QByteArray processedRequestMd5Hash, QByteArray json)
{
    c_myParser parser;

    myStructures::packet packet;
    packet.md5_hash = parser.getJsonMD5Hash( QJsonDocument::fromJson(json) );

    QDataStream ds2(&packet.packet_to_send, QIODevice::ReadWrite);
    ds2.setVersion(QDataStream::Qt_6_0);

    ds2 << packet.md5_hash.toHex() << json;

    sendDataToClient(packet);
    removeFromprocessedPackets(processedRequestMd5Hash);
}

bool c_ClientConnection::replyRequired(myStructures::threadData *request)
{
    bool replyRequired = true;

    return replyRequired;
}

void c_ClientConnection::waitForFinishProcessingTimerTimeOut()
{
}

void c_ClientConnection::parseReceivedPacket(quint64 size, QByteArray data, qintptr socketDescriptor)
{
    c_myParser parser;

    QPair<QByteArray, QByteArray> pair = parser.ParseReceivedPacket(size, data);
    myStructures::threadData jsonPacketData = parser.ParseJsonPacket( pair.second, socketDescriptor );

    myStructures::processedThreadData processedRequest;
    processedRequest.md5Hash = pair.first;
    processedRequest.socketDescriptor = jsonPacketData.socketDescriptor;
    processedRequest.content = jsonPacketData.content;
    processedRequest.thread_id = jsonPacketData.thread_id;
    processedRequest.thread_dest = jsonPacketData.thread_dest;
    processedRequest.thread_src = jsonPacketData.thread_src;
    processedRequest.req_type = jsonPacketData.req_type;
    processedRequest.type_flag = jsonPacketData.type_flag;
    processedRequest.data = jsonPacketData.data;

    myStructures::processedPacket processedPacket;
    processedPacket.md5_hash = pair.first;
    processedPacket.data = jsonPacketData;
    processedPacket.wait_for_reply = replyRequired(&jsonPacketData);

    QByteArray hex = pair.first.toHex();

    /*----------------------------------------------------*/

    myStructures::packet packet;
    QMap<QString, QVariant> packetInfo;
    packetInfo["thread_dest"] = static_cast<qint8>(myTypes::CLINIC_CONNECTION_CONTROLLER);
    packetInfo["thread_src"] = static_cast<qint8>(myTypes::SERVER);
    packetInfo["thread_id"] = processedRequest.thread_id;
    packetInfo["req_type"] = static_cast<qint8>(myTypes::RECEIVE_CONFIRMATION);
    packetInfo["type_flag"] = 0x00000001;
    packetInfo["content"] = static_cast<qint32>(myTypes::PACKET_RECEIVE_CONFIRMATION);

    QJsonDocument receiveConfirmation = parser.prepareReceiveConfirmationJSON(packetInfo, processedRequest.md5Hash);

    packet.wait_for_reply = false;
    packet.md5_hash = parser.getJsonMD5Hash( receiveConfirmation );

    QDataStream ds2(&packet.packet_to_send, QIODevice::ReadWrite);
    ds2.setVersion(QDataStream::Qt_6_0);

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

    QByteArray myPack;

    while(socket->canReadLine()) {
        QByteArray line = QByteArray( socket->readLine() );
        if( QString::fromUtf8(line) == QString("PACKET_BEGINNING\n") ) {
            myPack.clear();
        } else if( QString::fromUtf8(line) == QString("PACKET_END\n") ) {

            emit newLogToFile(QString("c_ClientConnection::readyRead"), QString("%1").arg(socketDescriptor), myPack);
            emit dataRead(myPack.size(), myPack, this->getSocketDescriptor());
        } else {
            myPack.append(line);
        }
    }
}

void c_ClientConnection::disconnected()
{
    while(executive->getProcessingMode()) {
        waitForFinishProcessingTimer->start(1000);
    }

    emit connectionFinished(this);
}

void c_ClientConnection::sendDataToClient(myStructures::packet packet)
{
        socket->write( QString("PACKET_BEGINNING\n").toUtf8() );
        socket->write( packet.packet_to_send );
        socket->write( QString("PACKET_END\n").toUtf8() );

        emit newLogToFile(QString("c_ClientConnection::reply(quint32 size, QByteArray data)"), QString("%1").arg(socket->socketDescriptor()), packet.packet_to_send);
}

