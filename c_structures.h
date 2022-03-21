#ifndef C_STRUCTURES_H
#define C_STRUCTURES_H

#include <QString>
#include <QUuid>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QByteArray>

enum SessionState {NOT_DEFINED = 0x00, DEFINED_NOT_STARTED = 0x01, STARTED = 0x10, RESTORED = 0x11, PAUSED_NOT_SAVED = 0x20, PAUSED_SAVED = 0x21, CLOSED_NOT_SAVED = 0x30, CLOSED_SAVED = 0x31};
enum ThreadDestination {SERVER = 0x00, CLINIC = 0x10, CLINIC_CONNECTION_CONTROLLER = 0x11};
enum RequestType {PING = 0x00, REPLY = 0x01, MESSAGE = 0x02, GET = 0x03, REQUEST = 0x04, UPDATE = 0x05, DELETE = 0x06, SEND = 0x07};

struct moduleInfo {
    QString name;
    QString displayedName;
    QString icon;
    QString description;
    QString pressedAction;
};

struct threadData {
    quint32 thread_id;
    ThreadDestination thread_dest;
    RequestType req_type;
    quint32 type_flag;
    quint32 data_size;
    QByteArray data;
};

struct sessionInformations {
    QUuid QUuid;
    SessionState state;
    quint64 userID;
    QDateTime startTime;
    QDateTime endTime;
    QString ipv4Address;
    QString macAddress;
    //QByteArray activity;
};


#endif // C_STRUCTURES_H
