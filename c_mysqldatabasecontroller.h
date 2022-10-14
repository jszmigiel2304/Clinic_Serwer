#ifndef C_MYSQLDATABASECONTROLLER_H
#define C_MYSQLDATABASECONTROLLER_H

#include "i_watched.h"
#include "c_mydatabaseconnection.h"

#include "mcs_DbQueriesUser.h"

#include <QObject>
#include <QList>
#include <QVariantList>
#include <QSql>
#include <QSqlDatabase>
#include <QMap>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlError>
#include <QSqlRecord>
#include <QCryptographicHash>

class c_MySqlDatabaseController : public QObject, public i_Watched
{
    Q_OBJECT
public:
    c_MySqlDatabaseController();
    c_MySqlDatabaseController(QMap<QString, QVariant> settings);
    c_MySqlDatabaseController(QMap<QString, QVariant> authDbSettings, QMap<QString, QVariant> clinicDbSettings);
    QMap<QString, QVariant> ShareProperties(QString sharedData = "all");
    void UpdateProperties(QMap<QString, QVariant> map);
    ~c_MySqlDatabaseController();

    void AddDatabase(QString name);
    void RemoveDatabase(QString name);
    void RemoveAllDatabases();
    void SetUpDatabase(QString name);
    void SetUpDatabase(QString name, QString hostName, quint16 port, QString databaseName, QString userName, QString password);
    void SetUpDatabase(QString name, QMap<QString, QVariant> settings);

    //QList<QVariantList> exe(QString query, QString destDatabase);

    QString getAuthDbName() const;
    void setAuthDbName(const QString &value);

    QString getAuthDbHostName() const;
    void setAuthDbHostName(const QString &value);

    quint16 getAuthDbPort() const;
    void setAuthDbPort(quint16 value);

    QString getClinicDbHostName() const;
    void setClinicDbHostName(const QString &value);

    quint16 getClinicDbPort() const;
    void setClinicDbPort(quint16 value);

    QString getClinicDbName() const;
    void setClinicDbName(const QString &value);

    QString getAuthDriver() const;
    void setAuthDriver(const QString &value);

    QString getAuthDbUserName() const;
    void setAuthDbUserName(const QString &value);

    QString getAuthPassword() const;
    void setAuthPassword(const QString &value);

    QString getClinicDriver() const;
    void setClinicDriver(const QString &value);

    QString getClinicDbUserName() const;
    void setClinicDbUserName(const QString &value);

    QString getClinicPassword() const;
    void setClinicPassword(const QString &value);

    QMap<QString, QSqlDatabase> getDatabases() const;
    void setDatabases(const QMap<QString, QSqlDatabase> &value);


public slots:
    void exe(QString query, QString destDatabase, QList<QMap<QString,QVariant>> * results, QStringList * errors = nullptr);
    void exe(QString query, QString destDatabase, QString name, QString password, QList<QMap<QString,QVariant>> * results, QStringList * errors = nullptr);



private:
    QMap<QString, QSqlDatabase> databases;

    QString authDriver;
    QString authDbHostName;
    quint16 authDbPort;
    QString authDbName;
    QString authDbUserName;
    QString authPassword;

    QString clinicDriver;
    QString clinicDbHostName;
    quint16 clinicDbPort;
    QString clinicDbName;
    QString clinicDbUserName;
    QString clinicPassword;

signals:
    void PropertiesChanged();
    void newLog(QString log);
    void newLogToFile(QString sender, QString notes, QByteArray data);

};

#endif // C_MYSQLDATABASE_H
