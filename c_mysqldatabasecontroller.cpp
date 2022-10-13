#include "c_mysqldatabasecontroller.h"



c_MySqlDatabaseController::c_MySqlDatabaseController()
{

}

c_MySqlDatabaseController::c_MySqlDatabaseController(QMap<QString, QVariant>)
{

}

c_MySqlDatabaseController::c_MySqlDatabaseController(QMap<QString, QVariant> authDbSettings, QMap<QString, QVariant> clinicDbSettings)
{
    this->authDriver = authDbSettings["driver"].toString();
    this->authDbHostName = authDbSettings["address"].toString();
    this->authDbPort = quint16(authDbSettings["port"].toInt());
    this->authDbName = authDbSettings["database_name"].toString();
    this->authDbUserName = authDbSettings["username"].toString();
    this->authPassword = authDbSettings["password"].toString();


    this->clinicDriver = clinicDbSettings["driver"].toString();
    this->clinicDbHostName = clinicDbSettings["address"].toString();
    this->clinicDbPort = quint16(clinicDbSettings["port"].toInt());
    this->clinicDbName = clinicDbSettings["database_name"].toString();
    this->clinicDbUserName = clinicDbSettings["username"].toString();
    this->clinicPassword = clinicDbSettings["password"].toString();
}

QMap<QString, QVariant> c_MySqlDatabaseController::ShareProperties(QString sharedData)
{
    QMap<QString, QVariant> map;


    if(sharedData == "all" || sharedData == "basicOnly" || sharedData == "authOnly")
    {
        map.insert("authDbStatus", QSqlDatabase::database("Authorization").isOpen() );
        map.insert("authDriver", this->authDriver);
        map.insert("authAddress", this->authDbHostName);
        map.insert("authPort", this->authDbPort);
        map.insert("authDbName", this->authDbName);
        map.insert("authPassword", this->authPassword);
        map.insert("authUserName", this->authDbUserName);

    }


    if(sharedData == "all" || sharedData == "basicOnly" || sharedData == "clinicOnly")
    {
        map.insert("clinicDbStatus", QSqlDatabase::database("Clinic").isOpen() );
        map.insert("clinicDriver", this->clinicDriver);
        map.insert("clinicAddress", this->clinicDbHostName);
        map.insert("clinicPort", this->clinicDbPort);
        map.insert("clinicDbName", this->clinicDbName);
        map.insert("clinicPassword", this->clinicPassword);
        map.insert("clinicUserName", this->clinicDbUserName);

    }


    return map;
}

void c_MySqlDatabaseController::UpdateProperties(QMap<QString, QVariant> map)
{
    this->authDriver = map["authDriver"].toString();
    this->authDbHostName = map["authAddress"].toString();
    this->authDbName = map["authDbName"].toString();
    this->authDbPort = quint16(map["authPort"].toInt());
    this->authDbUserName = map["authUserName"].toString();
    this->authPassword = map["authPassword"].toString();

    this->clinicDriver = map["clinicDriver"].toString();
    this->clinicDbHostName = map["clinicAddress"].toString();
    this->clinicDbName = map["clinicDbName"].toString();
    this->clinicDbPort = quint16(map["clinicPort"].toInt());
    this->clinicDbUserName = map["clinicUserName"].toString();
    this->clinicPassword = map["clinicPassword"].toString();

    emit this->PropertiesChanged();
}

c_MySqlDatabaseController::~c_MySqlDatabaseController()
{

}

void c_MySqlDatabaseController::AddDatabase(QString name)
{
    if(name == "Authorization")
        this->databases.insert(name, QSqlDatabase::addDatabase(authDriver, name));

    if(name == "Clinic")
        this->databases.insert(name, QSqlDatabase::addDatabase(clinicDriver, name));
}

void c_MySqlDatabaseController::RemoveDatabase(QString name)
{
    this->databases.remove(name);
    this->databases[name].removeDatabase(name);
    QSqlDatabase::removeDatabase(name);
}

void c_MySqlDatabaseController::RemoveAllDatabases()
{
    foreach (QString connectionName, QSqlDatabase::connectionNames()) {
        this->RemoveDatabase(connectionName);

    }
//    foreach (QSqlDatabase db, this->databases) {
//        this->RemoveDatabase(db.connectionName());
//    }
}

void c_MySqlDatabaseController::SetUpDatabase(QString name)
{
    QSqlDatabase::database(name).setHostName( ( (name == "Clinic") ? this->clinicDbHostName : this->authDbHostName  ) );
    QSqlDatabase::database(name).setPort( ( (name == "Clinic") ? this->clinicDbPort : this->authDbPort  ) );
    QSqlDatabase::database(name).setDatabaseName( ( (name == "Clinic") ? this->clinicDbName : this->authDbName  ) );
    QSqlDatabase::database(name).setUserName( ( (name == "Clinic") ? this->clinicDbUserName : this->authDbUserName  ) );
    QSqlDatabase::database(name).setPassword( ( (name == "Clinic") ? this->clinicPassword : this->authPassword  ) );
}

void c_MySqlDatabaseController::SetUpDatabase(QString name, QString hostName, quint16 port, QString databaseName, QString userName, QString password)
{
    QSqlDatabase::database(name).setHostName(hostName);
    QSqlDatabase::database(name).setPort(port);
    QSqlDatabase::database(name).setDatabaseName(databaseName);
    QSqlDatabase::database(name).setUserName(userName);
    QSqlDatabase::database(name).setPassword(password);
}

void c_MySqlDatabaseController::SetUpDatabase(QString name, QMap<QString, QVariant> settings)
{
    QSqlDatabase::database(name).setHostName(settings["address"].toString());
    QSqlDatabase::database(name).setPort(settings["port"].toInt());
    QSqlDatabase::database(name).setDatabaseName(settings["database_name"].toString());
    QSqlDatabase::database(name).setUserName(settings["username"].toString());
    QSqlDatabase::database(name).setPassword(settings["password"].toString());
}

void c_MySqlDatabaseController::exe(QString query, QString destDatabase, QList< QMap<QString,QVariant> > * results, QStringList * errors)
{


    if (databases[destDatabase].open())
    {
        QSqlQuery q(query, databases[destDatabase]);

        while (q.next()) {
                QSqlRecord record = q.record();

                QMap<QString,QVariant> map;
                for (int i=0; i<record.count(); ++i) {
                    map[record.fieldName(i)] = q.value(i);
                    //params.insert(record.fieldName(i++), q.value(i));
                }

                results->push_back(map);
            }

        QString err = q.lastError().text();

        databases[destDatabase].close();        
    } else {
        errors->push_back(QString("%1 Database opening error\n").arg(destDatabase));
    }


    //return resultsList;
}

void c_MySqlDatabaseController::exe(QString query, QString destDatabase, QString name, QString password, QList<QMap<QString, QVariant> > *results, QStringList *errors)
{
    if (databases[destDatabase].open(name, QString("md5<%1>").arg(password)))
    {
        QSqlQuery q(query, databases[destDatabase]);

        while (q.next()) {
                QSqlRecord record = q.record();

                QMap<QString,QVariant> map;
                for (int i=0; i<record.count(); ++i) {
                    map[record.fieldName(i)] = q.value(i);
                    //params.insert(record.fieldName(i++), q.value(i));
                }

                results->push_back(map);
            }

        QString err = q.lastError().text();

        databases[destDatabase].close();
    } else {
        errors->push_back(QString("%1 Database opening error\n").arg(destDatabase));
    }
}

void c_MySqlDatabaseController::setAuthDbName(const QString &value)
{
    authDbName = value;
}

void c_MySqlDatabaseController::setAuthDbHostName(const QString &value)
{
    authDbHostName = value;
}

void c_MySqlDatabaseController::setAuthDbPort(quint16 value)
{
    authDbPort = value;
}

QString c_MySqlDatabaseController::getAuthDbHostName() const
{
    return authDbHostName;
}

quint16 c_MySqlDatabaseController::getAuthDbPort() const
{
    return authDbPort;
}

QString c_MySqlDatabaseController::getAuthDbName() const
{
    return authDbName;
}

QString c_MySqlDatabaseController::getClinicDbHostName() const
{
    return clinicDbHostName;
}

void c_MySqlDatabaseController::setClinicDbHostName(const QString &value)
{
    clinicDbHostName = value;
}

quint16 c_MySqlDatabaseController::getClinicDbPort() const
{
    return clinicDbPort;
}

void c_MySqlDatabaseController::setClinicDbPort(quint16 value)
{
    clinicDbPort = value;
}

QString c_MySqlDatabaseController::getClinicDbName() const
{
    return clinicDbName;
}

void c_MySqlDatabaseController::setClinicDbName(const QString &value)
{
    clinicDbName = value;
}

QString c_MySqlDatabaseController::getAuthDriver() const
{
    return authDriver;
}

void c_MySqlDatabaseController::setAuthDriver(const QString &value)
{
    authDriver = value;
}

QString c_MySqlDatabaseController::getAuthDbUserName() const
{
    return authDbUserName;
}

void c_MySqlDatabaseController::setAuthDbUserName(const QString &value)
{
    authDbUserName = value;
}

QString c_MySqlDatabaseController::getAuthPassword() const
{
    return authPassword;
}

void c_MySqlDatabaseController::setAuthPassword(const QString &value)
{
    authPassword = value;
}

QString c_MySqlDatabaseController::getClinicDriver() const
{
    return clinicDriver;
}

void c_MySqlDatabaseController::setClinicDriver(const QString &value)
{
    clinicDriver = value;
}

QString c_MySqlDatabaseController::getClinicDbUserName() const
{
    return clinicDbUserName;
}

void c_MySqlDatabaseController::setClinicDbUserName(const QString &value)
{
    clinicDbUserName = value;
}

QString c_MySqlDatabaseController::getClinicPassword() const
{
    return clinicPassword;
}

void c_MySqlDatabaseController::setClinicPassword(const QString &value)
{
    clinicPassword = value;
}

QMap<QString, QSqlDatabase> c_MySqlDatabaseController::getDatabases() const
{
    return databases;
}

void c_MySqlDatabaseController::setDatabases(const QMap<QString, QSqlDatabase> &value)
{
    databases = value;
}

