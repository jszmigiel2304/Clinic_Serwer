#ifndef C_CLIENTCONNECTION2_H
#define C_CLIENTCONNECTION2_H

#include "w_logswindow.h"
#include "c_myparser.h"
#include "c_logscontroller.h"
#include "c_actionexecutive.h"

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

class c_ClientConnection2 : public QObject
{
    Q_OBJECT
public:
    explicit c_ClientConnection2(qintptr ID, QObject *parent = nullptr);
    ~c_ClientConnection2();


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
    void disconnect2();
    void connectionFinished(c_ClientConnection2 * client);
    void newClient(c_ClientConnection2 * client);
    void newLog(QString log);
    void newLogToFile(QString sender, QString notes, QByteArray data);

public slots:
    void readyRead();
    void disconnected();
    void sendDataToClient(quint64 size, QByteArray data);

private:
    c_actionExecutive * executive;

private slots:

};

#endif // C_CLIENTCONNECTION2_H
