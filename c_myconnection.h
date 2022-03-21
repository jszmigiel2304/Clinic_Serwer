#ifndef C_MYCONNECTION_H
#define C_MYCONNECTION_H

#include <QObject>
#include <QHostAddress>
#include <QTcpSocket>

class c_MyConnection : public QObject
{
    Q_OBJECT
public:
    explicit c_MyConnection(QObject *parent = nullptr);
    explicit c_MyConnection(QString HostAddress, quint16 port);

    QString getHostAddress() const;
    void setHostAddress(const QString &value);

    quint16 getPort() const;
    void setPort(const quint16 &value);

    virtual bool TestConnection(int time = 3000);

protected:
    QString HostAddress;
    quint16 Port;


signals:

public slots:
};

#endif // C_MYCONNECTION_H
