#ifndef C_USER_H
#define C_USER_H

#include "mcs_DbQueriesUser.h"
#include "c_structures.h"

#include <QObject>
#include <QDateTime>
#include <QHostAddress>
#include <QMetaEnum>
#include <QMetaType>

class c_User : public QObject
{
    Q_OBJECT
public:
    enum UserRole { NO_ROLE = 0x00, GUEST = 0x01, USER = 0x02, MODERATOR = 0x03, ADMIN = 0x04, SUPER = 0x05 };
    Q_ENUM(UserRole)

    c_User();
    c_User(qint32 user_id);
    c_User(QString user_login, QString user_encrypted_password);
    c_User(qint32 user_id, QString user_login, QString user_encrypted_password);
    //static qint32 getIDfromDataBase

    qint32 getId() const;

    QString getSelectIdQuery() const;
    QString getSelectUserQuery() const;
    QString getSelectByNamePasswordUserQuery() const;

    QString getInsertLoggingLogQuery(qint32 id_user, QDateTime log_time, QHostAddress ip_address, myTypes::USER_LOG_DB_ACTIVITY activity) const;

    QString getUpdateUserIsLoggedQuery(qint32 id_user, bool islogged);


    void setId(qint32 newId);

    const QString &getLogin() const;
    void setLogin(const QString &newLogin);

    const QString &getPassword() const;
    void setPassword(const QString &newPassword);

private:
    qint32 id;
    QString login;
    QString password;

signals:

};

#endif // C_USER_H
