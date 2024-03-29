
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


c_MySqlDatabaseController *c_actionExecutive::getDataBasesCtrlr() const
{
    return dataBasesCtrlr;
}

void c_actionExecutive::setDataBasesCtrlr(c_MySqlDatabaseController *newDataBasesCtrlr)
{
    dataBasesCtrlr = newDataBasesCtrlr;
    connect( this, SIGNAL(exeDataBaseQuery(QString, QString, QList<QMap<QString,QVariant>> *,QStringList *)), dataBasesCtrlr, SLOT(exe(QString, QString, QList<QMap<QString,QVariant>> *, QStringList *)) );
    connect( this, SIGNAL(exeDataBaseQuery(QString, QString, QString, QString, QList<QMap<QString,QVariant>> *,QStringList *)), dataBasesCtrlr, SLOT(exe(QString, QString, QString, QString, QList<QMap<QString,QVariant>> *, QStringList *)) );
}

void c_actionExecutive::dataReceived(myStructures::processedThreadData data)
{
    newDataReceivedFromClient(data);
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
        packetInfo["thread_dest"] = static_cast<qint8>(processedRequest.thread_src);
        packetInfo["thread_src"] = static_cast<qint8>(myTypes::SERVER);
        packetInfo["thread_id"] = processedRequest.thread_id;
        packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
        packetInfo["type_flag"] = 0x00000001;
        packetInfo["content"] = static_cast<qint32>(myTypes::USER_ID_ANSWER);
        packetInfo["ref_md5"] = QJsonValue::fromVariant( processedRequest.md5Hash.toHex() );

        QJsonDocument replyJSON = parser.prepareJSON(packetInfo, results);
        emit replyReady(processedRequest.md5Hash, replyJSON.toJson());

        break;
    }
    case 0x00000001: //GET user properties from database
    {
        c_myParser parser;

        QString name = processedRequest.data[0]["name"].toString();
        QString encryptedPassword = processedRequest.data[0]["encryptedPassword"].toString();

        c_User tempUser(name, encryptedPassword);

        QList< QMap<QString,QVariant> > results;
        QStringList errors;


        if(tempUser.getLogin().isEmpty() || tempUser.getPassword().isEmpty()) {
            (*processedRequestErrors)["INCORRECT_AUTHORIZATION_DATA\n"] = processedRequest;
            break;
        }

        emit exeDataBaseQuery(tempUser.getSelectByNamePasswordUserQuery(), QString("Authorization"), name, encryptedPassword, &results, &errors);


        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = processedRequest.thread_src;
        packetInfo["thread_src"] = static_cast<qint8>(myTypes::SERVER);
        packetInfo["thread_id"] = processedRequest.thread_id;
        packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
        packetInfo["type_flag"] = 0x00000001;
        packetInfo["content"] = static_cast<qint32>(myTypes::USER_PROPERTIES_ANSWER);
        packetInfo["ref_md5"] = QJsonValue::fromVariant( processedRequest.md5Hash.toHex() );

        QJsonDocument replyJSON = parser.prepareJSON(packetInfo, results);
        emit replyReady(processedRequest.md5Hash, replyJSON.toJson());
        break;
    }
    case 0x00000002: //GET employee properties from database
    {
        c_myParser parser;

        QString name = processedRequest.data[0]["name"].toString();
        QString encryptedPassword = processedRequest.data[0]["encryptedPassword"].toString();

        c_User tempUser(name, encryptedPassword);

        QList< QMap<QString,QVariant> > results;
        QStringList errors;


        if(tempUser.getLogin().isEmpty() || tempUser.getPassword().isEmpty()) {
            (*processedRequestErrors)["INCORRECT_AUTHORIZATION_DATA\n"] = processedRequest;
            break;
        }

        c_employee tempEmployee;

        emit exeDataBaseQuery(tempEmployee.getSelectEmployeePropertiesByUserNameQuery(tempUser.getLogin()), QString("Clinic"), name, encryptedPassword, &results, &errors);


        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = processedRequest.thread_src;
        packetInfo["thread_src"] = static_cast<qint8>(myTypes::SERVER);
        packetInfo["thread_id"] = processedRequest.thread_id;
        packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
        packetInfo["type_flag"] = 0x00000001;
        packetInfo["content"] = static_cast<qint32>(myTypes::EMPLOYEE_PROPERTIES_ANSWER);
        packetInfo["ref_md5"] = QJsonValue::fromVariant( processedRequest.md5Hash.toHex() );

        QJsonDocument replyJSON = parser.prepareJSON(packetInfo, results);
        emit replyReady(processedRequest.md5Hash, replyJSON.toJson());
        break;
    }
    case 0x00000003: //GET employee properties from database
    {
        c_myParser parser;

        QString name = processedRequest.data[0]["name"].toString();
        QString encryptedPassword = processedRequest.data[0]["encryptedPassword"].toString();
        quint32 employee_id = processedRequest.data[0]["employee_id"].toUInt();

        c_User tempUser(name, encryptedPassword);

        QList< QMap<QString,QVariant> > results;
        QStringList errors;


        if(tempUser.getLogin().isEmpty() || tempUser.getPassword().isEmpty()) {
            (*processedRequestErrors)["INCORRECT_AUTHORIZATION_DATA\n"] = processedRequest;
            break;
        }

        c_employee tempEmployee;

        emit exeDataBaseQuery(tempEmployee.getSelectEmployeePropertiesByIdQuery(employee_id), QString("Clinic"), name, encryptedPassword, &results, &errors);


        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = processedRequest.thread_src;
        packetInfo["thread_src"] = static_cast<qint8>(myTypes::SERVER);
        packetInfo["thread_id"] = processedRequest.thread_id;
        packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
        packetInfo["type_flag"] = 0x00000001;
        packetInfo["content"] = static_cast<qint32>(myTypes::EMPLOYEE_PROPERTIES_ANSWER);
        packetInfo["ref_md5"] = QJsonValue::fromVariant( processedRequest.md5Hash.toHex() );

        QJsonDocument replyJSON = parser.prepareJSON(packetInfo, results);
        emit replyReady(processedRequest.md5Hash, replyJSON.toJson());
        break;
    }
    case 0x00000004: //GET user logs from database
    {
        c_myParser parser;

        qint32 id = processedRequest.data[0]["id"].toInt();
        QString name = processedRequest.data[0]["name"].toString();
        QString encryptedPassword = processedRequest.data[0]["encryptedPassword"].toString();

        c_User tempUser(id, name, encryptedPassword);

        QList< QMap<QString,QVariant> > results;
        QStringList errors;


        if(tempUser.getLogin().isEmpty() || tempUser.getPassword().isEmpty()) {
            (*processedRequestErrors)["INCORRECT_AUTHORIZATION_DATA\n"] = processedRequest;
            break;
        }

        emit exeDataBaseQuery(tempUser.getSelectUserLogsQuery(), QString("Authorization"), name, encryptedPassword, &results, &errors);


        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = processedRequest.thread_src;
        packetInfo["thread_src"] = static_cast<qint8>(myTypes::SERVER);
        packetInfo["thread_id"] = processedRequest.thread_id;
        packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
        packetInfo["type_flag"] = 0x00000001;
        packetInfo["content"] = static_cast<qint32>(myTypes::USER_LOGS_ANSWER);
        packetInfo["ref_md5"] = QJsonValue::fromVariant( processedRequest.md5Hash.toHex() );

        QList<QMap<QString, QVariant>> logs;
        for(int i=0; i<results.size(); i++) {
            QMap<QString, QVariant> map;
            map["log"] = results[i]["activity"].toString();
            map["log_time"] = results[i]["activity"].toString();
            map["ip_address"] = results[i]["activity"].toString();
        }


        QJsonDocument replyJSON = parser.prepareJSON(packetInfo, logs);
        emit replyReady(processedRequest.md5Hash, replyJSON.toJson());
        break;
    }
    case 0x00000005: //GET user + employee logs from database
    {
        c_myParser parser;

        qint32 id = processedRequest.data[0]["id"].toInt();
        QString name = processedRequest.data[0]["name"].toString();
        QString encryptedPassword = processedRequest.data[0]["encryptedPassword"].toString();

        c_User tempUser(id, name, encryptedPassword);

        QList< QMap<QString,QVariant> > results;
        QStringList errors;


        if(tempUser.getLogin().isEmpty() || tempUser.getPassword().isEmpty()) {
            (*processedRequestErrors)["INCORRECT_AUTHORIZATION_DATA\n"] = processedRequest;
            break;
        }

        c_employee tempEmployee;

        emit exeDataBaseQuery(tempUser.getSelectUserLogsQuery(), QString("Authorization"), name, encryptedPassword, &results, &errors);
        emit exeDataBaseQuery(tempEmployee.getSelectEmployeeLogsByUserNameQuery(name), QString("Clinic"), name, encryptedPassword, &results, &errors);


        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = processedRequest.thread_src;
        packetInfo["thread_src"] = static_cast<qint8>(myTypes::SERVER);
        packetInfo["thread_id"] = processedRequest.thread_id;
        packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
        packetInfo["type_flag"] = 0x00000001;
        packetInfo["content"] = static_cast<qint32>(myTypes::USER_EMPLOYEE_LOGS_ANSWER);
        packetInfo["ref_md5"] = QJsonValue::fromVariant( processedRequest.md5Hash.toHex() );

        QList<myStructures::myLog> logs;
        for(int i=0; i<results.size(); i++) {
            myStructures::myLog log = myStructures::myLog::fromMap(results[i]);
            logs.append(log);
        }

        std::qsort(
                logs.data(),
                logs.size(),
                sizeof(myStructures::myLog),
                [](const void* x, const void* y) {
                    const QDateTime arg1 = (*static_cast<const myStructures::myLog*>(x)).time;
                    const QDateTime arg2 = (*static_cast<const myStructures::myLog*>(y)).time;
                    if (arg1 < arg2) return 1;
                    if (arg1 > arg2) return -1;
                    return 0;
                });

        QList<QMap<QString, QVariant>> mapsLogs;
        for(int i=0; i<logs.size(); i++) {
            mapsLogs.append(logs[i].toMap());
        }


        QJsonDocument replyJSON = parser.prepareJSON(packetInfo, mapsLogs);
        emit replyReady(processedRequest.md5Hash, replyJSON.toJson());
        break;
    }
    case 0x00000006: //GET employee logs from database
    {
        c_myParser parser;

        qint32 employee_id = processedRequest.data[0]["employee_id"].toInt();
        QString name = processedRequest.data[0]["name"].toString();
        QString encryptedPassword = processedRequest.data[0]["encryptedPassword"].toString();

        c_User tempUser(name, encryptedPassword);

        QList< QMap<QString,QVariant> > results;
        QStringList errors;


        if(tempUser.getLogin().isEmpty() || tempUser.getPassword().isEmpty()) {
            (*processedRequestErrors)["INCORRECT_AUTHORIZATION_DATA\n"] = processedRequest;
            break;
        }

        c_employee tempEmployee;

        emit exeDataBaseQuery(tempEmployee.getSelectEmployeeLogsByEmployeeIdQuery(employee_id), QString("Clinic"), name, encryptedPassword, &results, &errors);


        if( !errors.empty() ) {
            foreach(QString err, errors) {
                (*processedRequestErrors)[err] = processedRequest;
                break;
            }
        }

        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = processedRequest.thread_src;
        packetInfo["thread_src"] = static_cast<qint8>(myTypes::SERVER);
        packetInfo["thread_id"] = processedRequest.thread_id;
        packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
        packetInfo["type_flag"] = 0x00000001;
        packetInfo["content"] = static_cast<qint32>(myTypes::EMPLOYEE_LOGS_ANSWER);
        packetInfo["ref_md5"] = QJsonValue::fromVariant( processedRequest.md5Hash.toHex() );

        QList<QMap<QString, QVariant>> logs;
        for(int i=0; i<results.size(); i++) {
            QMap<QString, QVariant> map;
            map["log"] = results[i]["activity"].toString();
            map["log_time"] = results[i]["activity"].toString();
            map["ip_address"] = results[i]["activity"].toString();
        }

        QJsonDocument replyJSON = parser.prepareJSON(packetInfo, logs);
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
        packetInfo["thread_dest"] = processedRequest.thread_src;
        packetInfo["thread_src"] = static_cast<qint8>(myTypes::SERVER);
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
        // Incorret type_flag
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
        // Incorret type_flag
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
        // Incorret type_flag
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


        QList< QMap<QString,QVariant> > dataForJSON;

        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = processedRequest.thread_src;
        packetInfo["thread_src"] = static_cast<qint8>(myTypes::SERVER);
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

        QList< QMap<QString,QVariant> > dataForJSON;

        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = processedRequest.thread_src;
        packetInfo["thread_src"] = static_cast<qint8>(myTypes::SERVER);
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
        packetInfo["thread_dest"] = processedRequest.thread_src;
        packetInfo["thread_src"] = static_cast<qint8>(myTypes::SERVER);
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
        // Incorret type_flag
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

        QDateTime time = QDateTime::currentDateTime();

        QHostAddress address;
        if(dynamic_cast<c_ClientConnection *>(this->parent())->getSocket() != nullptr)
            address = dynamic_cast<c_ClientConnection *>(this->parent())->getSocket()->localAddress();
        else
            address = QHostAddress();

        emit exeDataBaseQuery(tempUser.getInsertLoggingLogQuery(tempUser.getId(), time, address, myTypes::SESSION_FILE_UPDATED), QString("Authorization"), &results, &errors);


        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = processedRequest.thread_src;
        packetInfo["thread_src"] = static_cast<qint8>(myTypes::SERVER);
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
        packetInfo["thread_dest"] = processedRequest.thread_src;
        packetInfo["thread_src"] = static_cast<qint8>(myTypes::SERVER);
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

        QMap<QString, QVariant> packetInfo;
        packetInfo["thread_dest"] = processedRequest.thread_src;
        packetInfo["thread_src"] = static_cast<qint8>(myTypes::SERVER);
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
        // Incorret type_flag
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
    packetInfo["thread_src"] = static_cast<qint8>(myTypes::SERVER);
    packetInfo["thread_id"] = errors->first().thread_id;
    packetInfo["req_type"] = static_cast<qint8>(myTypes::REPLY);
    packetInfo["type_flag"] = 0x00000001;
    packetInfo["content"] = static_cast<qint32>(myTypes::ERRORS);
    packetInfo["ref_md5"] = QJsonValue::fromVariant( errors->first().md5Hash.toHex() );

    QMap<QString, QVariant> map;


    int i = 0;
    foreach(const QString key, errors->keys()) {
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

