#include "c_user.h"

c_User::c_User()
{

}

c_User::c_User(qint32 user_id)
    : id{user_id}
{

}

c_User::c_User(QString user_login, QString user_encrypted_password)
    : login{user_login}, password{user_encrypted_password}
{

}

c_User::c_User(qint32 user_id, QString user_login, QString user_encrypted_password)
    : id{user_id}, login{user_login}, password{user_encrypted_password}
{

}

qint32 c_User::getId() const
{
    return id;
}

QString c_User::getSelectIdQuery() const
{
    QString query = QString(GET_USER_ID_AUTH_DB).arg(login, password);
    return query;
}

QString c_User::getInsertLoggingLogQuery(qint32 id_user, QDateTime log_time, QHostAddress ip_address, USER_LOG_DB_ACTIVITY activity) const
{
    QString activityStr;
    switch (static_cast<qint32>(activity))
    {
        case -1:    { activityStr = QString("UNKNOW"); break; }
        case 0x00000000:    { activityStr = QString("'LOGGING_ERROR'"); break; }
        case 0x00000001:    { activityStr = QString("'LOGGED_IN'"); break; }
        case 0x00000002:    { activityStr = QString("'LOGGED_OUT'"); break; }
        case 0x00010000:    { activityStr = QString("'SESSION_ERROR'"); break; }
        case 0x00010001:    { activityStr = QString("'SESSION_CREATED'"); break; }
        case 0x00010002:    { activityStr = QString("'SESSION_OPENED'"); break; }
        case 0x00010003:    { activityStr = QString("'SESSION_CLOSED'"); break; }
        case 0x00010004:    { activityStr = QString("'SESSION_RESTORED'"); break; }
        case 0x00010005:    { activityStr = QString("'SESSION_FILE_UPDATED'"); break; }
        default:    { activityStr = QString("'UNKNOW'"); break; }
    }

    QString query = QString(ADD_LOGGING_LOG_AUTH_DB).arg(QString("%1").arg(id_user), QString(log_time.toString()), QString(ip_address.toString()), activityStr);
    return query;
}

QString c_User::getUpdateUserIsLoggedQuery(qint32 id_user, bool islogged)
{
    QString query = QString(UPDATE_USER_IS_LOGGED_AUTH_DB).arg(QString("%1").arg(id_user), (islogged ? QString("true") : QString("false")) );
    return query;
}

QString c_User::getSelectUserQuery() const
{
    QString query = QString(GET_USER_AUTH_DB).arg(QString("%1").arg(id), login, password);
    return query;
}

void c_User::setId(qint32 newId)
{
    id = newId;
}

const QString &c_User::getLogin() const
{
    return login;
}

void c_User::setLogin(const QString &newLogin)
{
    login = newLogin;
}

const QString &c_User::getPassword() const
{
    return password;
}

void c_User::setPassword(const QString &newPassword)
{
    password = newPassword;
}
