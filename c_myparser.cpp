#include "c_myparser.h"

c_myParser::c_myParser(QObject *parent) : QObject(parent)
{

}

threadData c_myParser::Parse(quint32 size, QByteArray data)
{
    QString log = QString("Start Parsing");
    logs->addLog(log);

//    QByteArray hex = byteArray.toHex(' ');

//    logs->addSeparator();
//    logs->addLog(hex);
//    logs->addLog(QString("\nbytes: %1").arg(byteArray.length()));
//    logs->addSeparator();

//    c_myAction * action = new c_myAction();

//    QJsonDocument mJSON = QJsonDocument::fromBinaryData(data);


//        action->setAction( static_cast<Action>( (mJSON.object()).value("type").toInt() ) );
//        w_logsWindow::Instance()->addLog(   QString::number(   mJSON.object().value("type").toInt()    )     );
//        w_logsWindow::Instance()->addLog(   mJSON.object().value("name").toString()     );
//        w_logsWindow::Instance()->addLog(   mJSON.object().value("password").toString()     );


//    action->setJson( mJSON );

//    QJsonObject jObject;
//    jObject["type"] = 0;
//    jObject["login"] = "jszmigiel2304";
//    jObject["password"] = "passwrod";

//    mJSON.setObject(jObject);

//    action->setAction(LOGIN);
//    action->setJson(mJSON);


    threadData received_data;

    QDataStream rs(&data, QIODevice::ReadOnly);
    rs.setVersion(QDataStream::Qt_6_0);

    QByteArray attached_data;

    quint32 thread_id;
    quint8 thread_dest;
    quint32 attached_data_size;
    QByteArray attachedData;
    quint8 req_type;
    quint32 type_flag;

//    quint8 reqType;
//    QByteArray attachedData;
//    quint32 packet_size = size;

    rs >> thread_dest >> thread_id >> req_type >> type_flag >> attachedData;
    attached_data_size = attachedData.size();


    received_data.thread_id = thread_id;
    received_data.thread_dest = static_cast<ThreadDestination>(thread_dest);
    received_data.req_type = static_cast<RequestType>(req_type);
    received_data.type_flag = type_flag;
    received_data.data_size = attached_data_size;
    received_data.data = attachedData;

//    QDataStream rs2(&attachedData, QIODevice::ReadOnly);
//    rs2.setVersion(QDataStream::Qt_6_0);

//    quint8 type_flag;
//    QByteArray name;
//    QByteArray encryptedPassword;


//    rs2 >> type_flag >> name >> encryptedPassword;


    log = QString("End Parsing");
    logs->addLog(log);

    return received_data;
}

authenticator c_myParser::parseForAuthenticateData(quint32 size, QByteArray data)
{
    authenticator authData;

    QDataStream rs(&data, QIODevice::ReadOnly);
    rs.setVersion(QDataStream::Qt_6_0);

    QString name;
    QByteArray password;

    rs >> name >> password;

    authData.name = name;
    authData.password = password;

    return authData;
}

w_logsWindow *c_myParser::getLogs() const
{
    return logs;
}

void c_myParser::setLogs(w_logsWindow *value)
{
    logs = value;
}
