#ifndef C_SESSIONCONTROLLER_H
#define C_SESSIONCONTROLLER_H

#include "c_mysqldatabasecontroller.h"
#include "c_structures.h"
#include "c_user.h"
#include "mcs_DbQueriesSessionController.h"

#include <QObject>
#include <QList>
#include <QDateTime>
#include <QHostAddress>
#include <QUuid>
#include <QRandomGenerator>
#include <QFile>

class c_sessionController : QObject
{
    Q_OBJECT

public:
    c_sessionController();
    c_sessionController(c_User * user);
    ~c_sessionController();

    QString getInsertNewSessionQuery(QUuid id_session, qint32 id_user, QDateTime create_time, QHostAddress ip_address) const;
    QString getUpdateSessionActivityQuery(QUuid id_session, QByteArray data) const;
    QString getUpdateSessionStatusQuery(QUuid id_session, myTypes::SessionState state);
    QString getUpdateSessionCloseTimeQuery(QUuid id_session, QDateTime close_time);

    void setUpSession(QMap<QString, QVariant> map);
    QMap<QString, QVariant> getSettings();
    void setSessionUUID(QUuid uuid = QUuid());

    bool isValid();

    bool getOpened() const;
    void setOpened(bool newOpened);

    const QUuid &getIdentifier() const;
    void setIdentifier(const QUuid &newIdentifier);

    quint32 getFileControlNumber() const;
    void setFileControlNumber(quint32 newFileControlNumber);

    double getFileVersionNumber() const;
    void setFileVersionNumber(double newFileVersionNumber);

    const QString &getFileName() const;
    void setFileName(const QString &newFileName);

    const QString &getFilePath() const;
    void setFilePath(const QString &newFilePath);

    const QDateTime &getStartTime() const;
    void setStartTime(const QDateTime &newStartTime);

    const QDateTime &getCurrentOpenTime() const;
    void setCurrentOpenTime(const QDateTime &newCurrentOpenTime);

    myTypes::SessionState getState() const;
    void setState(myTypes::SessionState newState);


    c_User *getUser() const;
    void setUser(c_User *newUser);

    const QHostAddress &getHostAddress() const;
    void setHostAddress(const QHostAddress &newHostAddress);

    const QDateTime &getEndTime() const;
    void setEndTime(const QDateTime &newEndTime);

    const QByteArray &getFileData() const;
    void setFileData(const QByteArray &newFileData);

    quint32 getSessionExpireTime() const;

private:
    bool opened;
    QUuid identifier;
    quint32 fileControlNumber;
    double fileVersionNumber;
    QString fileName;
    QString filePath;
    QHostAddress hostAddress;
    QDateTime startTime;
    QDateTime endTime;
    QDateTime currentOpenTime;
    QByteArray fileData;
    const quint32 sessionExpireTime = SESSION_EXPIRE_TIME;

    myTypes::SessionState state;

    QFile sessionFile;

    c_User * user;

};

#endif // C_SESSIONCONTROLLER_H
