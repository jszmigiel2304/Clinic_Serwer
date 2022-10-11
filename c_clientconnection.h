#ifndef C_CLIENTCONNECTION_H
#define C_CLIENTCONNECTION_H


#include "w_logswindow.h"
#include "c_myparser.h"
#include "c_logscontroller.h"
#include "c_actionexecutive.h"
#include "c_structures.h"

#include <QObject>
#include <QThread>
#include <QSslSocket>
#include <QTextStream>
#include <QDataStream>
#include <QMessageBox>
#include <QJsonDocument>
#include <QByteArray>
#include <QBuffer>
#include <QIODeviceBase>
#include <QIODevice>
#include <QTimer>
#include <QJsonDocument>
#include <QDataStream>


class c_ClientConnection : public QObject
{
    Q_OBJECT
public:
    explicit c_ClientConnection(qintptr ID, QObject *parent = nullptr);
    ~c_ClientConnection();
    void removeFromprocessedPackets(QByteArray md5Hash);
    bool isProcessed(QByteArray md5Hash);

    QTcpSocket *socket;
    qintptr socketDescriptor;

    QTcpSocket *getSocket() const;
    void setSocket(QTcpSocket *newSocket);

    qintptr getSocketDescriptor() const;
    void setSocketDescriptor(qintptr newSocketDescriptor);

    c_actionExecutive *getExecutive() const;
    void setExecutive(c_actionExecutive *newExecutive);

signals:
    void error(QSslSocket::SocketError error);
    void dataRead(quint64 size, QByteArray data, qintptr socketDescriptor);
    void dataParsed(myStructures::processedThreadData data);
    void disconnect2();
    void connectionFinished(c_ClientConnection * client);
    void newClient(c_ClientConnection * client);
    void newLog(QString log);
    void newLogToFile(QString sender, QString notes, QByteArray data);    
    void reply(quint64 size, QByteArray data);
    void sendDataToClientSignal(myStructures::packet packet);

public slots:
    void readyRead();
    void disconnected();
//    void sendDataToClient(quint64 size, QByteArray data);
    void sendDataToClient(myStructures::packet packet);
    void replyReceived(QByteArray processedRequestMd5Hash, QByteArray json);

private:
    c_actionExecutive * executive;
    QTimer * waitForFinishProcessingTimer;

    QList<myStructures::processedPacket> processedPackets;

    bool replyRequired(myStructures::threadData * request);

private slots:
    void waitForFinishProcessingTimerTimeOut();
    void parseReceivedPacket(quint64 size, QByteArray data, qintptr socketDescriptor);

};

#endif // C_CLIENTCONNECTION_H
