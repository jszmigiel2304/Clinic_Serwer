#ifndef C_CLIENTCONNECTION_H
#define C_CLIENTCONNECTION_H

#include "w_logswindow.h"
#include "c_myparser.h"
#include "c_logscontroller.h"

#include <QObject>
#include <QThread>
#include <QSslSocket>
#include <QTextStream>
#include <QDataStream>
#include <QMessageBox>
#include <QJsonDocument>
#include <QByteArray>
#include <QBuffer>


class c_ClientConnection : public QThread
{
    Q_OBJECT

public:
    explicit c_ClientConnection(qintptr ID, QObject *parent = nullptr);
    ~c_ClientConnection();

    void run();
    void myClose();
    void myClose2();

    void reply(QByteArray answer);

    QSslSocket *getSocket() const;
    void setSocket(QSslSocket *value);

    qintptr getSocketDescriptor() const;
    void setSocketDescriptor(const qintptr &value);

protected:

signals:
    void error(QSslSocket::SocketError error);
    void dataRead(quint32 size, QByteArray data);

public slots:
    void readyRead();
    void disconnected();

private:
    QSslSocket *socket;
    qintptr socketDescriptor;    

    w_logsWindow *logs;
};

#endif // C_CLIENTCONNECTION_H
