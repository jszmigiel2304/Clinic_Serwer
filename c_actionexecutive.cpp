#include "c_actionexecutive.h"
#include "c_clientconnection.h"

c_actionExecutive::c_actionExecutive(QObject *parent) : QObject(parent)
{
    setProcessingMode(false);
    processedRequestErrors = new QMap<QString, myStructures::processedThreadData>();


    connect(this, SIGNAL(newDataInBuffor()), this, SLOT(startProcessingRequests()));
    connect(this, SIGNAL(processingErrors(QMap<QString, myStructures::processedThreadData> *)), this, SLOT(processErrors(QMap<QString, myStructures::processedThreadData> *)));
}

c_actionExecutive::~c_actionExecutive()
{    
    delete processedRequestErrors;
}

void c_actionExecutive::newDataReceivedFromClient(myStructures::processedThreadData data)
{
    receivedDataFromClients.push_back(data);

    emit newDataInBuffor();
}

w_logsWindow *c_actionExecutive::getLogs() const
{
    return logs;
}

void c_actionExecutive::setLogs(w_logsWindow *value)
{
    logs = value;
}



c_MySqlDatabaseController *c_actionExecutive::getDataBasesCtrlr() const
{
    return dataBasesCtrlr;
}

void c_actionExecutive::setDataBasesCtrlr(c_MySqlDatabaseController *newDataBasesCtrlr)
{
    dataBasesCtrlr = newDataBasesCtrlr;
    connect( this, SIGNAL(exeDataBaseQuery(QString, QString, QList<QMap<QString,QVariant>> *,QStringList *)), dataBasesCtrlr, SLOT(exe(QString, QString, QList<QMap<QString,QVariant>> *, QStringList *)) );
}

void c_actionExecutive::dataReceived(myStructures::processedThreadData data)
{

    newDataReceivedFromClient(data);

    QString log = QString("Data received (Total Data in buffer %1) \n").arg(this->getReceivedDataFromClients().size());
    emit newLog(log);

}

void c_actionExecutive::startProcessingRequests()
{
    if(getProcessingMode()) return;
    processRequests(this->processedRequestErrors);

}

void c_actionExecutive::processGetRequest(myStructures::processedThreadData processedRequest, QMap<QString, myStructures::processedThreadData> *processedRequestErrors)
{
    switch (processedRequest.type_flag) {
    case 0x00000000:  //getUserID from database
    {
        c_myParser parser;

        QString name = processedRequest.data[0]["name"].toString();
        QString encryptedPassword = processedRequest.data[0]["encryptedPassword"].toString();

        c_User tempUser(name, encryptedPassword);

        QList< QMap<QString,QVariant> > results;
        QStringList errors;


        if(tempUser.getLogin().isEmpty() || tempUser.getPassword().isEmpty()) {
            (*processedRequestErrors)["INCORRECT_LOGIN_DATA\n"] = processedRequest;
            break;
        }

        emit exeDataBaseQuery(tempUser.getSelectIdQuery(), QString("Authorization"), &results, &errors);


        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        if( results.empty() ) {
            QMap<QString, QVariant> resultMap;
            resultMap["id"] = -1;
            results.append(resultMap);
        }


        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = static_cast<qint8>(myTypes::CLINIC_LOGGED_USER_CONTROLLER);
        packetInfo["thread_id"] = processedRequest.thread_id;
        packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
        packetInfo["type_flag"] = 0x00000001;
        packetInfo["content"] = static_cast<qint32>(myTypes::USER_ID_ANSWER);
        packetInfo["ref_md5"] = QJsonValue::fromVariant( processedRequest.md5Hash.toHex() );

        QJsonDocument replyJSON = parser.prepareJSON(packetInfo, results);
        emit replyReady(processedRequest.md5Hash, replyJSON.toJson());

        break;
    }
    case 0x00000100:  //getSessionssettings from server+database
    {

        c_myParser parser;

        qint32 id = processedRequest.data[0]["id"].toInt();
        QString name = processedRequest.data[0]["name"].toString();
        QString encryptedPassword = processedRequest.data[0]["encryptedPassword"].toString();
        QUuid session_identifier = processedRequest.data[0]["session_identifier"].toUuid();

        c_User tempUser(id, name, encryptedPassword);

        QList< QMap<QString,QVariant> > results;
        QStringList errors;

        if(tempUser.getId() < 0 || tempUser.getLogin().isEmpty() || tempUser.getPassword().isEmpty()) {
            (*processedRequestErrors)["INCORRECT_LOGIN_DATA\n"] = processedRequest;
            break;
        }

        emit exeDataBaseQuery(tempUser.getSelectUserQuery(), QString("Authorization"), &results, &errors);


        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        if( results.empty() ) {
            (*processedRequestErrors)["INCORRECT_LOGIN_DATA\n"] = processedRequest;
            break;
        }

        c_sessionController tempSession(&tempUser);
        if( !session_identifier.isNull() )
            tempSession.setSessionUUID(session_identifier);
        QDateTime timestamp = QDateTime::currentDateTime();

        emit exeDataBaseQuery(tempSession.getInsertNewSessionQuery(tempSession.getIdentifier(),
                                                                   tempUser.getId(),
                                                                   timestamp,
                                                                   dynamic_cast<c_ClientConnection *>(this->parent())->getSocket()->localAddress()),
                              QString("Authorization"),
                              &results,
                              &errors);

        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        if( results.empty() ) {
            (*processedRequestErrors)["ADD_NEW_RECORD_DB_ERROR\n"] = processedRequest;
            break;
        }

        QMap<QString, QVariant> map = results[1];
        tempSession.setUpSession(map);
        QList< QMap<QString,QVariant> > dataForJSON;
        dataForJSON.push_back(tempSession.getSettings());

        QDateTime time = QDateTime::currentDateTime();

        QHostAddress address;
        if(dynamic_cast<c_ClientConnection *>(this->parent())->getSocket() != nullptr)
            address = dynamic_cast<c_ClientConnection *>(this->parent())->getSocket()->localAddress();
        else
            address = QHostAddress();

        emit exeDataBaseQuery(tempUser.getInsertLoggingLogQuery(tempUser.getId(), time, address, myTypes::SESSION_CREATED), QString("Authorization"), &results, &errors);

        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = static_cast<qint8>(myTypes::CLINIC_SESSION_CONTROLLER);
        packetInfo["thread_id"] = processedRequest.thread_id;
        packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
        packetInfo["type_flag"] = 0x00000001;
        packetInfo["content"] = static_cast<qint32>(myTypes::NEW_SESSION_SETTINGS);
        packetInfo["ref_md5"] = QJsonValue::fromVariant( processedRequest.md5Hash.toHex() );


        QJsonDocument replyJSON = parser.prepareJSON(packetInfo, dataForJSON);
        emit replyReady(processedRequest.md5Hash, replyJSON.toJson());



        break;
    }
    case 0x00000101: //getUsersUnfinishedSessions from database
    {
        break;
    }
    default:
    {
        emit newLog( QString("c_actionExecutive::processGetRequest(...): Incorrect TYPE_FLAG[%1]\n").arg(processedRequest.type_flag) );
        break;
    }

    }
}

void c_actionExecutive::processMessageRequest(myStructures::processedThreadData processedRequest, QMap<QString, myStructures::processedThreadData> *processedRequestErrors)
{

    switch (processedRequest.type_flag) {
    case 0x00000000:  //Text message
    {
        break;
    }
    default:
    {
        emit newLog( QString("c_actionExecutive::processMessageRequest(...): Incorrect TYPE_FLAG[%1]\n").arg(processedRequest.type_flag) );
        break;
    }
    }
}

void c_actionExecutive::processConfirmRequest(myStructures::processedThreadData processedRequest, QMap<QString, myStructures::processedThreadData> *processedRequestErrors)
{
    switch (processedRequest.type_flag) {
    case 0x00000000:
    {
        break;
    }
    case 0x00000001:
    {

        break;
    }
    default:
    {
        emit newLog( QString("c_actionExecutive::processConfirmRequest(...): Incorrect TYPE_FLAG[%1]\n").arg(processedRequest.type_flag) );
        break;
    }
    }

}

void c_actionExecutive::processRequestRequest(myStructures::processedThreadData processedRequest, QMap<QString, myStructures::processedThreadData> *processedRequestErrors)
{
    switch (processedRequest.type_flag) {
    case 0x00000000:  // logIn request
    {
        // sprawdz dane logowanie z baza danych + dodaj log do bazy danych + wyslij potwierdzenie
        c_myParser parser;

        qint32 id = processedRequest.data[0]["id"].toInt();
        QString name = processedRequest.data[0]["name"].toString();
        QString encryptedPassword = processedRequest.data[0]["encryptedPassword"].toString();

        c_User tempUser(id, name, encryptedPassword);

        QList< QMap<QString,QVariant> > results;
        QStringList errors;

        if(tempUser.getId() < 0 || tempUser.getLogin().isEmpty() || tempUser.getPassword().isEmpty()) {
            (*processedRequestErrors)["INCORRECT_LOGIN_DATA\n"] = processedRequest;
            break;
        }

        emit exeDataBaseQuery(tempUser.getSelectUserQuery(), QString("Authorization"), &results, &errors);


        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        if( results.empty() ) {
            (*processedRequestErrors)["INCORRECT_LOGIN_DATA\n"] = processedRequest;
            break;
        }

        QDateTime time = QDateTime::currentDateTime();

        QHostAddress address;
        if(dynamic_cast<c_ClientConnection *>(this->parent())->getSocket() != nullptr)
            address = dynamic_cast<c_ClientConnection *>(this->parent())->getSocket()->localAddress();
        else
            address = QHostAddress();

        emit exeDataBaseQuery(tempUser.getInsertLoggingLogQuery(tempUser.getId(), time, address, myTypes::LOGGED_IN), QString("Authorization"), &results, &errors);
        emit exeDataBaseQuery(tempUser.getUpdateUserIsLoggedQuery(tempUser.getId(), true), QString("Authorization"), &results, &errors);

        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        emit newLog( QString("Log in confirmation: Log successfully added to db.\n") );


        QList< QMap<QString,QVariant> > dataForJSON;

        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = static_cast<qint8>(myTypes::CLINIC_LOGGED_USER_CONTROLLER);
        packetInfo["thread_id"] = processedRequest.thread_id;
        packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
        packetInfo["type_flag"] = 0x00000001;
        packetInfo["content"] = static_cast<qint32>(myTypes::LOGGING_ANSWER);
        packetInfo["ref_md5"] = QJsonValue::fromVariant( processedRequest.md5Hash.toHex() );

        QMap<QString, QVariant> map;
        map["id"] = results[0]["id"].toInt();
        map["name"] = results[0]["name"].toString();
        map["email"] = results[0]["email"].toString();
        map["verified"] = results[0]["verified"].toBool();
        map["blocked"] = results[0]["blocked"].toBool();
        map["create_date"] = results[0]["create_date"].toString();
        map["verify_date"] = results[0]["verify_date"].toString();
        map["block_date"] = results[0]["block_date"].toString();
        map["photo"] = results[0]["photo"].toByteArray();
        map["logInTime"] = time.toString();
        map["role"] = results[0]["role"].toString();
        dataForJSON.append(map);

        QJsonDocument replyJSON = parser.prepareJSON(packetInfo, dataForJSON);
        emit replyReady(processedRequest.md5Hash, replyJSON.toJson());

        break;
    }
    case 0x00000001:  // logOutRequest
    {
        // sprawdz dane logowanie z baza danych + dodaj log do bazy danych + wyslij potwierdzenie
        c_myParser parser;

        qint32 id = processedRequest.data[0]["id"].toInt();
        QString name = processedRequest.data[0]["name"].toString();
        QString encryptedPassword = processedRequest.data[0]["encryptedPassword"].toString();

        c_User tempUser(id, name, encryptedPassword);

        QList< QMap<QString,QVariant> > results;
        QStringList errors;

        if(tempUser.getId() < 0 || tempUser.getLogin().isEmpty() || tempUser.getPassword().isEmpty()) {
            (*processedRequestErrors)["INCORRECT_LOGIN_DATA\n"] = processedRequest;
            break;
        }

        emit exeDataBaseQuery(tempUser.getSelectUserQuery(), QString("Authorization"), &results, &errors);

        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        if( results.empty() ) {
            (*processedRequestErrors)["INCORRECT_LOGIN_DATA\n"] = processedRequest;
            break;
        }

        QDateTime time = QDateTime::currentDateTime();

        QHostAddress address;
        if(dynamic_cast<c_ClientConnection *>(this->parent())->getSocket() != nullptr)
            address = dynamic_cast<c_ClientConnection *>(this->parent())->getSocket()->localAddress();
        else
            address = QHostAddress();

        emit exeDataBaseQuery(tempUser.getInsertLoggingLogQuery(tempUser.getId(),
                                                                time,
                                                                address,
                                                                myTypes::LOGGED_OUT),
                              QString("Authorization"),
                              &results,
                              &errors);
        emit exeDataBaseQuery(tempUser.getUpdateUserIsLoggedQuery(tempUser.getId(), false), QString("Authorization"), &results, &errors);

        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        emit newLog( QString("Log out confirmation: Log successfully added to db.\n") );


        QList< QMap<QString,QVariant> > dataForJSON;

        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = static_cast<qint8>(myTypes::CLINIC_LOGGED_USER_CONTROLLER);
        packetInfo["thread_id"] = processedRequest.thread_id;
        packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
        packetInfo["type_flag"] = 0x00000001;
        packetInfo["content"] = static_cast<qint32>(myTypes::LOGGING_OUT_ANSWER);
        packetInfo["ref_md5"] = QJsonValue::fromVariant( processedRequest.md5Hash.toHex() );

        QMap<QString, QVariant> map;
        map["id"] = results[0]["id"].toInt();
        map["name"] = results[0]["name"].toString();
        map["logOutTime"] = time.toString();
        dataForJSON.append(map);

        QJsonDocument replyJSON = parser.prepareJSON(packetInfo, dataForJSON);
        emit replyReady(processedRequest.md5Hash, replyJSON.toJson());

        break;
    }
    case 0x00000002:  //unlock on idle
    {
        c_myParser parser;

        QString name = processedRequest.data[0]["name"].toString();
        QString encryptedPassword = processedRequest.data[0]["encryptedPassword"].toString();

        c_User tempUser(name, encryptedPassword);

        QList< QMap<QString,QVariant> > results;
        QStringList errors;


        if(tempUser.getLogin().isEmpty() || tempUser.getPassword().isEmpty()) {
            (*processedRequestErrors)["INCORRECT_LOGIN_DATA\n"] = processedRequest;
            break;
        }

        emit exeDataBaseQuery(tempUser.getSelectIdQuery(), QString("Authorization"), &results, &errors);


        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        if( results.empty() ) {
            (*processedRequestErrors)["INCORRECT_LOGIN_DATA\n"] = processedRequest;
            break;
        }

        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = static_cast<qint8>(myTypes::CLINIC_LOGGED_USER_CONTROLLER);
        packetInfo["thread_id"] = processedRequest.thread_id;
        packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
        packetInfo["type_flag"] = 0x00000000;
        packetInfo["content"] = static_cast<qint32>(myTypes::SESSION_UNLOCK_CONFIRMATION);
        packetInfo["ref_md5"] = QJsonValue::fromVariant( processedRequest.md5Hash.toHex() );

        QJsonDocument replyJSON = parser.prepareJSON(packetInfo, QList<QMap<QString, QVariant>>());
        emit replyReady(processedRequest.md5Hash, replyJSON.toJson());

        break;
    }
    default:
    {
        emit newLog( QString("c_actionExecutive::processRequestRequest(...): Incorrect TYPE_FLAG[%1]\n").arg(processedRequest.type_flag) );
        break;
    }
    }
}

void c_actionExecutive::processUpdateRequest(myStructures::processedThreadData processedRequest, QMap<QString, myStructures::processedThreadData> *processedRequestErrors)
{
    switch (processedRequest.type_flag) {
    case 0x01000001: // update session file data
    {
        c_myParser parser;

        qint32 id = processedRequest.data[0]["id"].toInt();
        QString name = processedRequest.data[0]["name"].toString();
        QString encryptedPassword = processedRequest.data[0]["encryptedPassword"].toString();
        QUuid session_identifier = processedRequest.data[0]["session_identifier"].toUuid();
        QByteArray session_file_data = QByteArray::fromHex( processedRequest.data[0]["file_data"].toString().toUtf8() );

        c_User tempUser(id, name, encryptedPassword);

        QList< QMap<QString,QVariant> > results;
        QStringList errors;

        if(tempUser.getId() < 0 || tempUser.getLogin().isEmpty() || tempUser.getPassword().isEmpty()) {
            (*processedRequestErrors)["INCORRECT_LOGIN_DATA\n"] = processedRequest;
            break;
        }

        emit exeDataBaseQuery(tempUser.getSelectUserQuery(), QString("Authorization"), &results, &errors);


        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        if( results.empty() ) {
            (*processedRequestErrors)["INCORRECT_LOGIN_DATA\n"] = processedRequest;
            break;
        }

        c_sessionController tempSession(&tempUser);


        emit exeDataBaseQuery(tempSession.getUpdateSessionActivityQuery(session_identifier, session_file_data),
                              QString("Authorization"),
                              &results,
                              &errors);

        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        if( results.empty() ) {
            (*processedRequestErrors)["UPDATE_RECORD_DB_ERROR\n"] = processedRequest;
            break;
        }

        emit newLog( QString("Update confirmation: Session activity succesfully updated.\n") );

        QDateTime time = QDateTime::currentDateTime();

        QHostAddress address;
        if(dynamic_cast<c_ClientConnection *>(this->parent())->getSocket() != nullptr)
            address = dynamic_cast<c_ClientConnection *>(this->parent())->getSocket()->localAddress();
        else
            address = QHostAddress();

        emit exeDataBaseQuery(tempUser.getInsertLoggingLogQuery(tempUser.getId(), time, address, myTypes::SESSION_FILE_UPDATED), QString("Authorization"), &results, &errors);


        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = static_cast<qint8>(myTypes::CLINIC_SESSION_CONTROLLER);
        packetInfo["thread_id"] = processedRequest.thread_id;
        packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
        packetInfo["type_flag"] = 0x00000000;
        packetInfo["content"] = static_cast<qint32>(myTypes::SESSION_FILE_UPDATE_CONFIRMATION);
        packetInfo["ref_md5"] = QJsonValue::fromVariant( processedRequest.md5Hash.toHex() );

        QJsonDocument replyJSON = parser.prepareJSON(packetInfo, QList<QMap<QString, QVariant>>());
        emit replyReady(processedRequest.md5Hash, replyJSON.toJson());

        break;

    }
    case 0x01000002: //update session status
    {
        c_myParser parser;

        qint32 id = processedRequest.data[0]["id"].toInt();
        QString name = processedRequest.data[0]["name"].toString();
        QString encryptedPassword = processedRequest.data[0]["encryptedPassword"].toString();
        QUuid session_identifier = processedRequest.data[0]["session_identifier"].toUuid();
        myTypes::SessionState session_state = static_cast<myTypes::SessionState>( processedRequest.data[0]["session_state"].toInt() );

        c_User tempUser(id, name, encryptedPassword);

        QList< QMap<QString,QVariant> > results;
        QStringList errors;

        if(tempUser.getId() < 0 || tempUser.getLogin().isEmpty() || tempUser.getPassword().isEmpty()) {
            (*processedRequestErrors)["INCORRECT_LOGIN_DATA\n"] = processedRequest;
            break;
        }

        emit exeDataBaseQuery(tempUser.getSelectUserQuery(), QString("Authorization"), &results, &errors);


        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        if( results.empty() ) {
            (*processedRequestErrors)["INCORRECT_LOGIN_DATA\n"] = processedRequest;
            break;
        }

        c_sessionController tempSession(&tempUser);

        if(session_identifier.isNull()) {
            (*processedRequestErrors)["INCORRECT_DATA_ATTACHED_ERROR\n"] = processedRequest;
            break;
        }

        emit exeDataBaseQuery(tempSession.getUpdateSessionStatusQuery(session_identifier, session_state),
                              QString("Authorization"),
                              &results,
                              &errors);

        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        if( results.empty() ) {
            (*processedRequestErrors)["UPDATE_RECORD_DB_ERROR\n"] = processedRequest;
            break;
        }

        emit newLog( QString("Update confirmation: Session status succesfully updated.\n") );

        QDateTime time = QDateTime::currentDateTime();

        QHostAddress address;
        if(dynamic_cast<c_ClientConnection *>(this->parent())->getSocket() != nullptr)
            address = dynamic_cast<c_ClientConnection *>(this->parent())->getSocket()->localAddress();
        else
            address = QHostAddress();

        switch(session_state) {
        case myTypes::DEFINED_NOT_STARTED: {
            emit exeDataBaseQuery(tempUser.getInsertLoggingLogQuery(tempUser.getId(), time, address, myTypes::SESSION_CREATED), QString("Authorization"), &results, &errors);
            break;
        }
        case myTypes::STARTED: {
            emit exeDataBaseQuery(tempUser.getInsertLoggingLogQuery(tempUser.getId(), time, address, myTypes::SESSION_OPENED), QString("Authorization"), &results, &errors);
            break;
        }
        case myTypes::RESTORED: {
            emit exeDataBaseQuery(tempUser.getInsertLoggingLogQuery(tempUser.getId(), time, address, myTypes::SESSION_RESTORED), QString("Authorization"), &results, &errors);
            break;
        }
        case myTypes::CLOSED_SAVED: {
            emit exeDataBaseQuery(tempUser.getInsertLoggingLogQuery(tempUser.getId(), time, address, myTypes::SESSION_CLOSED), QString("Authorization"), &results, &errors);
            break;
        }
        case myTypes::ERROR: {
            emit exeDataBaseQuery(tempUser.getInsertLoggingLogQuery(tempUser.getId(), time, address, myTypes::SESSION_ERROR), QString("Authorization"), &results, &errors);
            break;
        }
        default: {
            break;
        }
        }

        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = static_cast<qint8>(myTypes::CLINIC_SESSION_CONTROLLER);
        packetInfo["thread_id"] = processedRequest.thread_id;
        packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
        packetInfo["type_flag"] = 0x00000000;
        packetInfo["content"] = static_cast<qint32>(myTypes::SESSION_STATE_UPDATE_CONFIRMATION);

        packetInfo["ref_md5"] = QJsonValue::fromVariant( processedRequest.md5Hash.toHex() );

        QJsonDocument replyJSON = parser.prepareJSON(packetInfo, QList<QMap<QString, QVariant>>());
        emit replyReady(processedRequest.md5Hash, replyJSON.toJson());
        break;

    }
    case 0x01000003: //Update Session Close Time
    {
        c_myParser parser;

        qint32 id = processedRequest.data[0]["id"].toInt();
        QString name = processedRequest.data[0]["name"].toString();
        QString encryptedPassword = processedRequest.data[0]["encryptedPassword"].toString();
        QUuid session_identifier = processedRequest.data[0]["session_identifier"].toUuid();
        QDateTime close_time =  QDateTime::fromString(processedRequest.data[0]["close_time"].toString());

        c_User tempUser(id, name, encryptedPassword);

        QList< QMap<QString,QVariant> > results;
        QStringList errors;

        if(tempUser.getId() < 0 || tempUser.getLogin().isEmpty() || tempUser.getPassword().isEmpty()) {
            (*processedRequestErrors)["INCORRECT_LOGIN_DATA\n"] = processedRequest;
            break;
        }

        emit exeDataBaseQuery(tempUser.getSelectUserQuery(), QString("Authorization"), &results, &errors);


        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        if( results.empty() ) {
            (*processedRequestErrors)["INCORRECT_LOGIN_DATA\n"] = processedRequest;
            break;
        }

        c_sessionController tempSession(&tempUser);

        if(session_identifier.isNull()) {
            (*processedRequestErrors)["INCORRECT_DATA_ATTACHED_ERROR\n"] = processedRequest;
            break;
        }

        emit exeDataBaseQuery(tempSession.getUpdateSessionCloseTimeQuery(session_identifier, close_time),
                              QString("Authorization"),
                              &results,
                              &errors);

        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        if( results.empty() ) {
            (*processedRequestErrors)["UPDATE_RECORD_DB_ERROR\n"] = processedRequest;
            break;
        }

        emit newLog( QString("Update confirmation: Session close_time succesfully updated.\n") );

        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = static_cast<qint8>(myTypes::CLINIC_SESSION_CONTROLLER);
        packetInfo["thread_id"] = processedRequest.thread_id;
        packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
        packetInfo["type_flag"] = 0x00000000;
        packetInfo["content"] = static_cast<qint32>(myTypes::SESSION_CLOSE_TIME_UPDATE_CONFIRMATION);

        packetInfo["ref_md5"] = QJsonValue::fromVariant( processedRequest.md5Hash.toHex() );

        QJsonDocument replyJSON = parser.prepareJSON(packetInfo, QList<QMap<QString, QVariant>>());
        emit replyReady(processedRequest.md5Hash, replyJSON.toJson());
        break;
    }
    default:
    {
        emit newLog( QString("c_actionExecutive::processRequestRequest(...): Incorrect TYPE_FLAG[%1]\n").arg(processedRequest.type_flag) );
        break;
    }
    }
}

void c_actionExecutive::processErrors(QMap<QString, myStructures::processedThreadData> *errors)
{
    c_myParser parser;

    QList< QMap<QString,QVariant> > dataForJSON;

    QMap<QString, QVariant> packetInfo;
    packetInfo["thread_dest"] = static_cast<qint8>(myTypes::CLINIC_ERROR_CONTROLLER);
    packetInfo["thread_id"] = errors->first().thread_id;
    packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
    packetInfo["type_flag"] = 0x00000001;
    packetInfo["content"] = static_cast<qint32>(myTypes::ERRORS);
    packetInfo["ref_md5"] = QJsonValue::fromVariant( errors->first().md5Hash.toHex() );

    QMap<QString, QVariant> map;


    int i = 0;
    foreach(const QString key, errors->keys()) {
        emit newLog(QString("ERROR: %1 \n").arg(key));
        map[QString("ERROR.%1").arg(i)] = key;
        i++;
    }


    dataForJSON.append(map);
    QJsonDocument replyJSON = parser.prepareJSON(packetInfo, dataForJSON);
    emit replyReady(errors->first().md5Hash, replyJSON.toJson());

}


bool c_actionExecutive::getProcessingMode() const
{
    return processingMode;
}

void c_actionExecutive::setProcessingMode(bool newProcessingMode)
{
    processingMode = newProcessingMode;
}

void c_actionExecutive::processRequests(QMap<QString, myStructures::processedThreadData> *processedRequestErrors)
{
    while(receivedDataFromClients.length() > 0)
    {
        setProcessingMode(true);

        // obsługa żądań

        switch (getReceivedDataFromClients()[0].req_type) {
        case myTypes::PING:
        {

        }
        case myTypes::REPLY:
        {

        }
        case myTypes::MESSAGE:
        {
            processMessageRequest(receivedDataFromClients.takeAt(0), processedRequestErrors);
            break;
        }
        case myTypes::GET:
        {
            processGetRequest(receivedDataFromClients.takeAt(0), processedRequestErrors);
            break;
        }
        case myTypes::REQUEST:
        {
            processRequestRequest(receivedDataFromClients.takeAt(0), processedRequestErrors);
            break;
        }
        case myTypes::UPDATE:
        {
            processUpdateRequest(receivedDataFromClients.takeAt(0), processedRequestErrors);
            break;
        }
        case myTypes::DELETE:
        {

        }
        case myTypes::SEND:
        {

        }
        case myTypes::CONFIRM:
        {
            processConfirmRequest(receivedDataFromClients.takeAt(0), processedRequestErrors);
            break;
        }
        default:
        {

            break;
        }
        }

        if( !processedRequestErrors->empty() ) {
            emit processingErrors(processedRequestErrors);
            processedRequestErrors->clear();
        }
    }


    setProcessingMode(false);
}

const QList<myStructures::processedThreadData> &c_actionExecutive::getReceivedDataFromClients() const
{
    return receivedDataFromClients;
}

void c_actionExecutive::setReceivedDataFromClients(const QList<myStructures::processedThreadData> &newReceivedDataFromClients)
{
    receivedDataFromClients = newReceivedDataFromClients;
}

