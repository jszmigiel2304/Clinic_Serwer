#include "c_actionexecutive.h"
#include "c_clinictcpserver.h"

c_actionExecutive::c_actionExecutive(QObject *parent) : QObject(parent)
{
    setProcessingMode(false);
    processedRequestErrors = new QMap<threadData, QString>();

    connect(this, SIGNAL(newDataInBuffor()), this, SLOT(startProcessingRequests()));
}

c_actionExecutive::~c_actionExecutive()
{
    delete processedRequestErrors;
}

void c_actionExecutive::newDataReceivedFromClient(threadData data)
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

void c_actionExecutive::log_in(QAuthenticator auth, QByteArray * answer)
{
//    QList<QVariant> vars;

//    QSqlQuery query( ( (dynamic_cast<c_ClinicTcpServer *>(parent()))->getDbContr() )->getDatabases()["Authorization"] );
//    query.prepare("SELECT id FROM public.users WHERE name=:name AND password=:password");
//    query.bindValue(":name", auth.user());
//    query.bindValue(":password", auth.password());

//    if(!query.exec()) {
//        QSqlError aaaa = query.lastError();
//    } else {
//        if (query.size() >0) {
//            while (query.next()) {
//                QJsonDocument answerJSON;
//                QJsonObject jObj;
//                jObj["type"] = static_cast<int>(REPLY);
//                jObj["id_user"] = query.value("id").toInt();
//                //jObj["id_session"] = query.value("id").toString();
//                answerJSON.setObject(jObj);
//                answer->append(answerJSON.toBinaryData());

//                //tworzenie sesji
//            }
//        } else {
//            QString msg = "Nie znaleziono takiego użytkownika";
//            answer->append(msg);
//        }

    //    }
}

void c_actionExecutive::startProcessingRequests()
{

    if(getProcessingMode()) return;
    processRequests(this->processedRequestErrors);

}

void c_actionExecutive::processGetRequest(threadData processedRequest, QMap<threadData, QString> *processedRequestErrors)
{
    switch (processedRequest.type_flag) {
    case 0x00000000:  //getSessionUUID
    {
        //szukanie otwartych sesji
        c_sessionController sessionCtrlr(qobject_cast<c_ClinicTcpServer *>(parent())->getDbContr());
        c_myParser parser;

        authenticator authData = parser.parseForAuthenticateData(processedRequest.data_size, processedRequest.data);

        QString u = authData.name;
        QByteArray ptolatin = authData.password;
        QString pstrtgin = authData.password;

        sessionCtrlr.getSessionsFromDataBase(authData.name, authData.password);
        break;
    }
    default:
    {
        break;
    }

    }
}

bool c_actionExecutive::getProcessingMode() const
{
    return processingMode;
}

void c_actionExecutive::setProcessingMode(bool newProcessingMode)
{
    processingMode = newProcessingMode;
}

void c_actionExecutive::processRequests(QMap<threadData, QString> *processedRequestErrors)
{
    while(receivedDataFromClients.length() > 0)
    {
        setProcessingMode(true);

        // obsługa żądań

        switch (getReceivedDataFromClients()[0].req_type) {
        case PING:
        {
            break;
        }
        case REPLY:
        {
            break;
        }
        case MESSAGE:
        {
            break;
        }
        case GET:
        {
            processGetRequest(getReceivedDataFromClients()[0], processedRequestErrors);
            break;
        }
        case REQUEST:
        {
            break;
        }
        case UPDATE:
        {
            break;
        }
        case DELETE:
        {
            break;
        }
        case SEND:
        {
            break;
        }
        default:
        {
            break;
        }
        }
        // usunięcię wykonanego zadania
        receivedDataFromClients.removeAt(0);
    }

    setProcessingMode(false);
}

const QList<threadData> &c_actionExecutive::getReceivedDataFromClients() const
{
    return receivedDataFromClients;
}

void c_actionExecutive::setReceivedDataFromClients(const QList<threadData> &newReceivedDataFromClients)
{
    receivedDataFromClients = newReceivedDataFromClients;
}

