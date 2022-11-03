#ifndef C_MYDATABASECONNECTION_H
#define C_MYDATABASECONNECTION_H

#include "c_myconnection.h"
#include "m_databasedrivers.h"

#include <QObject>
#include <QSqlDatabase>

class c_MyDatabaseConnection : public c_MyConnection
{
    Q_OBJECT
public:
    explicit c_MyDatabaseConnection();
    explicit c_MyDatabaseConnection(QString hostAddress, quint16 port, QString driver, QString databaseName, QString userName, QString password, QObject * parent = nullptr);

    QString getDriver() const;
    void setDriver(const QString &value);

    QString getDatabaseName() const;
    void setDatabaseName(const QString &value);

    QString getUserName() const;
    void setUserName(const QString &value);

    QString getPassword() const;
    void setPassword(const QString &value);

    bool TestConnection(int time = 3000);

private:
    QString Driver;
    QString DatabaseName;
    QString UserName;
    QString Password;

signals:
    void newLogToFile(QString sender, QString notes, QByteArray data);

public slots:
};

#endif // C_MYDATABASECONNECTION_H
