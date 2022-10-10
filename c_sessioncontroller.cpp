#include "c_sessioncontroller.h"

c_sessionController::c_sessionController()
{
}

c_sessionController::c_sessionController(c_User * user)
{
    this->user = user;
}

c_sessionController::~c_sessionController()
{

}

QString c_sessionController::getInsertNewSessionQuery(QUuid id_session, qint32 id_user, QDateTime create_time, QHostAddress ip_address) const
{
    QString query = QString(ADD_NEW_SESSION_AUTH_DB).arg(id_session.toString(QUuid::WithoutBraces), QString("%1").arg(id_user), create_time.toString(), ip_address.toString());
    return query;
}

QString c_sessionController::getUpdateSessionActivityQuery(QUuid id_session, QByteArray data) const
{
    QString query = QString(UPDATE_SESSION_ACTIVITY_AUTH_DB).arg(QByteArray::fromHex(data), id_session.toString(QUuid::WithoutBraces));
    return query;
}

QString c_sessionController::getUpdateSessionStatusQuery(QUuid id_session, SessionState state)
{
    QString stateString;
    switch(static_cast<qint32>(state)) {
    case NOT_DEFINED: { stateString = QString("NOT_DEFINED"); break; }
    case DEFINED_NOT_STARTED: { stateString = QString("DEFINED_NOT_STARTED"); break; }
    case STARTED: { stateString = QString("STARTED"); break; }
    case RESTORED: { stateString = QString("RESTORED"); break; }
    case RESTARTED: { stateString = QString("RESTARTED"); break; }
    case PAUSED_NOT_SAVED: { stateString = QString("PAUSED_NOT_SAVED"); break; }
    case PAUSED_SAVED: { stateString = QString("PAUSED_SAVED"); break; }
    case CLOSED_NOT_SAVED: { stateString = QString("CLOSED_NOT_SAVED"); break; }
    case CLOSED_SAVED: { stateString = QString("CLOSED_SAVED"); break; }
    default: { stateString = QString("ERROR"); break; }
    }

    QString query = QString(UPDATE_SESSION_STATE_AUTH_DB).arg(id_session.toString(QUuid::WithoutBraces), stateString);
    return query;
}

QString c_sessionController::getUpdateSessionCloseTimeQuery(QUuid id_session, QDateTime close_time)
{
    QString query = QString(UPDATE_SESSION_CLOSE_TIME_AUTH_DB).arg(id_session.toString(QUuid::WithoutBraces), close_time.toString());
    return query;
}

void c_sessionController::setUpSession(QMap<QString, QVariant> map)
{
    double fileVersionNumber = 1.0001;
    quint32 fileControlNumber = quint32( QRandomGenerator( QDateTime::currentMSecsSinceEpoch() ).generate() );

    QUuid uuid = map["id"].toString().isEmpty() ? QUuid() : QUuid::fromString( map["id"].toString());

    QString filePath = QString("sessions//");
    QString fileName = QString(uuid.toString(QUuid::WithoutBraces)).append(".ssa");

    QPair<QHostAddress, int> pair = QHostAddress::parseSubnet( map["ip_address"].toString());
    QHostAddress hostAddr = pair.first;


    QDateTime start = map["begin_time"].toString().isEmpty() ? QDateTime() : QDateTime::fromString(map["begin_time"].toString(), Qt::ISODate);
    QDateTime end = map["end_time"].toString().isEmpty() ? QDateTime() : QDateTime::fromString(map["end_time"].toString(), Qt::ISODate);


    if(map["state"].toString() != "STARTED")
        setOpened(false);
    else
        setOpened(true);

    setIdentifier( uuid );
    setFileControlNumber(fileControlNumber);
    setFileVersionNumber(fileVersionNumber);
    setFileName(fileName);
    setFilePath(filePath);
    setHostAddress(hostAddr);
    setStartTime(start);
    setEndTime(end);

    if(map["state"].toString() == "NOT_DEFINED") setState(NOT_DEFINED);
    if(map["state"].toString() == "DEFINED_NOT_STARTED") setState(DEFINED_NOT_STARTED);
    if(map["state"].toString() == "STARTED") setState(STARTED);
    if(map["state"].toString() == "RESTORED") setState(RESTORED);
    if(map["state"].toString() == "RESTARTED") setState(RESTARTED);
    if(map["state"].toString() == "PAUSED_NOT_SAVED") setState(PAUSED_NOT_SAVED);
    if(map["state"].toString() == "PAUSED_SAVED") setState(PAUSED_SAVED);
    if(map["state"].toString() == "CLOSED_NOT_SAVED") setState(CLOSED_NOT_SAVED);
    if(map["state"].toString() == "CLOSED_SAVED") setState(CLOSED_SAVED);
    if(map["state"].toString() == "ERROR") setState(ERROR);

}

void c_sessionController::setSessionUUID(QUuid uuid)
{
    setIdentifier(uuid);
}

QMap<QString, QVariant> c_sessionController::getSettings()
{
    QMap<QString, QVariant> map;

    if(isValid()) {
        map["id"] = identifier.toString(QUuid::WithoutBraces);
        map["id_user"] = user->getId();
        map["begin_time"] = startTime.toString();
        map["end_time"] = endTime.toString();
        map["ip_address"] = hostAddress.toString();
        map["state"] = static_cast<quint8>(getState());
        map["file_version_number"] = fileVersionNumber;
        map["file_control_number"] = fileControlNumber;
        map["file_name"] = fileName;
        map["file_path"] = filePath;
        map["session_expire_time"] = sessionExpireTime;
    }

    return map;
}

bool c_sessionController::isValid()
{
    if(identifier.isNull()) return false;
    if(fileControlNumber == 0) return false;
    if(fileVersionNumber == 0) return false;
    if(fileName.isEmpty()) return false;
    if(filePath.isEmpty()) return false;
    if(startTime.isNull()) return false;
    if(static_cast<quint8>(state) == 0xff) return false;
    if(user == nullptr) return false;

    return true;
}

bool c_sessionController::getOpened() const
{
    return opened;
}

void c_sessionController::setOpened(bool newOpened)
{
    opened = newOpened;
}

const QUuid &c_sessionController::getIdentifier() const
{
    return identifier;
}

void c_sessionController::setIdentifier(const QUuid &newIdentifier)
{
    identifier = newIdentifier;
}

quint32 c_sessionController::getFileControlNumber() const
{
    return fileControlNumber;
}

void c_sessionController::setFileControlNumber(quint32 newFileControlNumber)
{
    fileControlNumber = newFileControlNumber;
}

double c_sessionController::getFileVersionNumber() const
{
    return fileVersionNumber;
}

void c_sessionController::setFileVersionNumber(double newFileVersionNumber)
{
    fileVersionNumber = newFileVersionNumber;
}

const QString &c_sessionController::getFileName() const
{
    return fileName;
}

void c_sessionController::setFileName(const QString &newFileName)
{
    fileName = newFileName;
}

const QString &c_sessionController::getFilePath() const
{
    return filePath;
}

void c_sessionController::setFilePath(const QString &newFilePath)
{
    filePath = newFilePath;
}

const QDateTime &c_sessionController::getStartTime() const
{
    return startTime;
}

void c_sessionController::setStartTime(const QDateTime &newStartTime)
{
    startTime = newStartTime;
}

const QDateTime &c_sessionController::getCurrentOpenTime() const
{
    return currentOpenTime;
}

void c_sessionController::setCurrentOpenTime(const QDateTime &newCurrentOpenTime)
{
    currentOpenTime = newCurrentOpenTime;
}

SessionState c_sessionController::getState() const
{
    return state;
}

void c_sessionController::setState(SessionState newState)
{
    state = newState;
}


c_User *c_sessionController::getUser() const
{
    return user;
}

void c_sessionController::setUser(c_User *newUser)
{
    user = newUser;
}

const QHostAddress &c_sessionController::getHostAddress() const
{
    return hostAddress;
}

void c_sessionController::setHostAddress(const QHostAddress &newHostAddress)
{
    hostAddress = newHostAddress;
}

const QDateTime &c_sessionController::getEndTime() const
{
    return endTime;
}

void c_sessionController::setEndTime(const QDateTime &newEndTime)
{
    endTime = newEndTime;
}

const QByteArray &c_sessionController::getFileData() const
{
    return fileData;
}

void c_sessionController::setFileData(const QByteArray &newFileData)
{
    fileData = newFileData;
}

quint32 c_sessionController::getSessionExpireTime() const
{
    return sessionExpireTime;
}
