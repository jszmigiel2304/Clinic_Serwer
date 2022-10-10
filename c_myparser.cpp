#include "c_myparser.h"

c_myParser::c_myParser(QObject *parent) : QObject(parent)
{

}


QPair<QByteArray, QByteArray> c_myParser::ParseReceivedPacket(quint64 size, QByteArray data/*, qintptr socketDescriptor*/)
{
    QDataStream rs(&data, QIODevice::ReadOnly);
    rs.setVersion(QDataStream::Qt_6_0);

    QByteArray md5Hash;
    QByteArray jsonPacket;

    rs >> md5Hash >> jsonPacket;


    QPair<QByteArray, QByteArray> pair;
    pair.first = QByteArray::fromHex(md5Hash);
    pair.second = jsonPacket;
    return pair;
}

threadData c_myParser::ParseJsonPacket(QByteArray json, qintptr socketDescriptor)
{
    threadData packetData;

    packetData.socketDescriptor = socketDescriptor;

    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json, &error);

    if (error.error != QJsonParseError::NoError) {
        packetData.content = ERRORS;
        QList<QMap<QString, QVariant>> attData;
        QMap<QString, QVariant> error;
        error["error_0"] = QString("JSON reading error.");
        attData.append(error);
        return packetData;
    }

    QJsonObject mainObject = jsonDoc.object();


    packetData.thread_dest = static_cast<ThreadDestination>( mainObject["thread_dest"].toInteger() );
    packetData.thread_id = mainObject["thread_id"].toInteger();
    packetData.req_type = static_cast<RequestType>( mainObject["req_type"].toInteger() );
    packetData.type_flag = mainObject["type_flag"].toInteger();
    packetData.content = static_cast<JsonContent>( mainObject["content"].toInteger() );


    QJsonArray dataArray = mainObject["data"].toArray();

    for(int i = 0; i < dataArray.size(); i++)
    {
        QJsonObject object = dataArray[i].toObject();

        QMap<QString, QVariant> map;
        foreach(QString key, object.keys()) {
            map[key] = object[key].toVariant();
        }
        packetData.data.append(map);
    }

    return packetData;
}



void c_myParser::parseJsonForAuthenticateData(const QJsonDocument *json, JsonContent *content, qint32 *id, QString *name, QByteArray *password, QMap<QString, QVariant> *additionalData)
{
    QJsonObject mainObject = json->object();

    *content = static_cast<JsonContent>( mainObject["CONTENT"].toInteger() );

    QJsonObject resultObject = mainObject["RESULTS"].toObject();   

    if(*content == 0xff000100) {
        foreach (const QString key, resultObject.keys() ) {
            if(key == "ID") {
                if (resultObject["ID"].toInt() > -1) { *id = resultObject["ID"].toInt(); }
            }
            if(key == "NAME") {
                if (!resultObject["NAME"].toString().isEmpty()) { *name = resultObject["NAME"].toString(); }
            }
            if(key == "PASSWORD") {
                if (!resultObject["PASSWORD"].toString().toUtf8().isEmpty()) { *password = QByteArray::fromHex( resultObject["PASSWORD"].toString().toUtf8() ); }
            }
            (*additionalData)[key] = resultObject[key].toVariant();
        }
    }  else {
        if (resultObject["ID"].toInt() > -1) { *id = resultObject["ID"].toInt(); }
        if (!resultObject["NAME"].toString().isEmpty()) { *name = resultObject["NAME"].toString(); }
        if (!resultObject["PASSWORD"].toString().toUtf8().isEmpty()) { *password = QByteArray::fromHex( resultObject["PASSWORD"].toString().toUtf8() ); }
    }

}

void c_myParser::parseJsonForAuthenticateData(const QJsonDocument *json, JsonContent *content, QString *name, QByteArray *password, QMap<QString, QVariant> *additionalData)
{
    QJsonObject mainObject = json->object();

    *content = static_cast<JsonContent>( mainObject["CONTENT"].toInteger() );

    QJsonObject resultObject = mainObject["RESULTS"].toObject();

    if(*content == 0xff000100) {
        foreach (const QString key, resultObject.keys() ) {
            if(key == "NAME") {
                if (!resultObject["NAME"].toString().isEmpty()) { *name = resultObject["NAME"].toString(); }
            }
            if(key == "PASSWORD") {
                if (!resultObject["PASSWORD"].toString().toUtf8().isEmpty()) { *password = QByteArray::fromHex( resultObject["PASSWORD"].toString().toUtf8() ); }
            }
            (*additionalData)[key] = resultObject[key].toVariant();
        }
    }  else {
        if (!resultObject["NAME"].toString().isEmpty()) { *name = resultObject["NAME"].toString(); }
        if (!resultObject["PASSWORD"].toString().toUtf8().isEmpty()) { *password = QByteArray::fromHex( resultObject["PASSWORD"].toString().toUtf8() ); }
    }
}

void c_myParser::parseJsonForAuthenticateData(const QJsonDocument *json, JsonContent *content, authenticator *auth, QMap<QString, QVariant> *additionalData)
{
    QJsonObject mainObject = json->object();

    *content = static_cast<JsonContent>( mainObject["CONTENT"].toInteger() );

    QJsonObject resultObject = mainObject["RESULTS"].toObject();

    if(*content == 0xff000100) {
        foreach (const QString key, resultObject.keys() ) {
            if(key == "ID") {
                if (resultObject["ID"].toInt() > -1) { (*auth).id = resultObject["ID"].toInt(); }
            }
            if(key == "NAME") {
                if (!resultObject["NAME"].toString().isEmpty()) { (*auth).name = resultObject["NAME"].toString(); }
            }
            if(key == "PASSWORD") {
                if (!resultObject["PASSWORD"].toString().toUtf8().isEmpty()) { (*auth).password = QByteArray::fromHex( resultObject["PASSWORD"].toString().toUtf8() ); }
            }
            (*additionalData)[key] = resultObject[key].toVariant();
        }
    }  else {
        if (resultObject["ID"].toInt() > -1) { (*auth).id = resultObject["ID"].toInt(); }
        if (!resultObject["NAME"].toString().isEmpty()) { (*auth).name = resultObject["NAME"].toString(); }
        if (!resultObject["PASSWORD"].toString().toUtf8().isEmpty()) { (*auth).password  = QByteArray::fromHex( resultObject["PASSWORD"].toString().toUtf8() ); }
    }

}

void c_myParser::parseJsonForMessaegeText(const QJsonDocument *json, JsonContent *content, QString *message)
{
    QJsonObject mainObject = json->object();

    *content = static_cast<JsonContent>( mainObject["CONTENT"].toInteger() );
    *message = mainObject["RESULTS"].toString();
}

void c_myParser::parseJsonForBinaryData(const QJsonDocument *json, JsonContent *content, QByteArray *binaries)
{
    QJsonObject mainObject = json->object();

    *content = static_cast<JsonContent>( mainObject["CONTENT"].toInteger() );

    QJsonObject resultObject = mainObject["RESULTS"].toObject();

    if (!resultObject["DATA"].toString().toUtf8().isEmpty()) { (*binaries) = QByteArray::fromHex( resultObject["DATA"].toString().toUtf8() ); }
}

void c_myParser::parseJsonConfirmation(const QJsonDocument *json, QMap<QString, QVariant> *confirmationData)
{
    QJsonObject mainObject = json->object();

    foreach (const QString key, mainObject.keys() ) {
        (*confirmationData)[key] = mainObject[key].toVariant();
    }
}

QJsonDocument c_myParser::prepareJSON(QMap<QString, QVariant> packet_info, QList<QMap<QString, QVariant>> packet_data)
{
    QJsonDocument jsonDoc;
    QJsonObject mainobject;


    QJsonArray packetDataArray;

    foreach(QString key, packet_info.keys()) {
        mainobject[key] = QJsonValue::fromVariant(packet_info[key]);
    }


    for(int i = 0; i < packet_data.size(); i++) {
        QJsonObject data_obj;

        foreach(QString key, packet_data[i].keys()) {
            data_obj[key] = QJsonValue::fromVariant(packet_data[i][key]);
        }

        packetDataArray.append(data_obj);
    }

    mainobject["data"] = packetDataArray;
    jsonDoc.setObject(mainobject);
    return jsonDoc;


}

QJsonDocument c_myParser::prepareReceiveConfirmationJSON(QMap<QString, QVariant> packet_info, QByteArray md5Hash)
{
    QJsonDocument jsonDoc;
    QJsonObject mainobject;


    QJsonArray packetDataArray;

    foreach(QString key, packet_info.keys()) {
        mainobject[key] = QJsonValue::fromVariant(packet_info[key]);
    }

    QJsonObject data_obj;
    data_obj["md5"] = QJsonValue::fromVariant( md5Hash.toHex() );
    packetDataArray.append(data_obj);


    mainobject["data"] = packetDataArray;
    jsonDoc.setObject(mainobject);
    return jsonDoc;
}


w_logsWindow *c_myParser::getLogs() const
{
    return logs;
}

void c_myParser::setLogs(w_logsWindow *value)
{
    logs = value;
}

QByteArray c_myParser::getJsonMD5Hash(QJsonDocument json)
{
    QByteArray md5Hash;

    QCryptographicHash hasher(QCryptographicHash::Md5);
    hasher.addData(json.toJson());
    md5Hash = hasher.result();

    return md5Hash;
}
