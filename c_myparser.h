#ifndef C_MYPARSER_H
#define C_MYPARSER_H

#include "c_structures.h"

#include <QObject>
#include <QMap>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>
#include <QtEndian>
#include <QAuthenticator>
#include <QPair>
#include <QCryptographicHash>

class c_myParser : public QObject
{
    Q_OBJECT
public:
    explicit c_myParser(QObject *parent = nullptr);


    QPair<QByteArray, QByteArray> ParseReceivedPacket(quint64 size, QByteArray data/*, qintptr socketDescriptor*/);
    myStructures::threadData ParseJsonPacket(QByteArray json, qintptr socketDescriptor);

    void parseJsonForAuthenticateData(const QJsonDocument *json, myTypes::JsonContent *content, qint32 *id, QString *name, QByteArray *password, QMap<QString, QVariant> *additionalData = nullptr);
    void parseJsonForAuthenticateData(const QJsonDocument *json, myTypes::JsonContent *content, QString *name, QByteArray *password, QMap<QString, QVariant> *additionalData = nullptr);
    void parseJsonForAuthenticateData(const QJsonDocument *json, myTypes::JsonContent *content, myStructures::authenticator *auth, QMap<QString, QVariant> *additionalData = nullptr);
    void parseJsonForMessaegeText(const QJsonDocument *json, myTypes::JsonContent *content, QString *message);
    void parseJsonForBinaryData(const QJsonDocument *json, myTypes::JsonContent *content, QByteArray *binaries);
    void parseJsonConfirmation(const QJsonDocument *json, QMap<QString, QVariant> *confirmationData);


    QJsonDocument prepareJSON(QMap<QString, QVariant> packet_info, QList<QMap<QString, QVariant>> packet_data);
    QJsonDocument prepareReceiveConfirmationJSON(QMap<QString, QVariant> packet_info, QByteArray md5Hash);

    QByteArray getJsonMD5Hash(QJsonDocument json);

signals:

public slots:

private:

};

#endif // C_MYPARSER_H
