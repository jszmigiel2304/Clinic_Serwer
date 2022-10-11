#ifndef C_ACTIONEXECUTIVE_H
#define C_ACTIONEXECUTIVE_H

#include "w_logswindow.h"
#include "c_structures.h"
#include "c_sessioncontroller.h"
#include "c_mysqldatabasecontroller.h"
#include "c_myparser.h"
#include "c_user.h"

#include <QObject>
#include <QAuthenticator>
#include <QMap>
#include <QList>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlResult>
#include <QJsonDocument>
#include <QDateTime>
#include <QPair>

class c_actionExecutive : public QObject
{
    Q_OBJECT
public:
    explicit c_actionExecutive(QObject *parent = nullptr);
    ~c_actionExecutive();


    w_logsWindow *getLogs() const;
    void setLogs(w_logsWindow *value);

    const QList<myStructures::processedThreadData> &getReceivedDataFromClients() const;
    void setReceivedDataFromClients(const QList<myStructures::processedThreadData> &newReceivedDataFromClients);

    bool getProcessingMode() const;
    void setProcessingMode(bool newProcessingMode);

    void processRequests(QMap<QString, myStructures::processedThreadData> * processedRequestErrors = nullptr);

    c_MySqlDatabaseController *getDataBasesCtrlr() const;

public slots:
    void newDataReceivedFromClient(myStructures::processedThreadData data);
    void setDataBasesCtrlr(c_MySqlDatabaseController *newDataBasesCtrlr);
    void dataReceived(myStructures::processedThreadData data);

protected:

private:
    QList<myStructures::processedThreadData> receivedDataFromClients;
    QMap<QString, myStructures::processedThreadData> * processedRequestErrors;

    c_MySqlDatabaseController * dataBasesCtrlr;

    w_logsWindow *logs;

    bool processingMode;

private slots:
    void startProcessingRequests();
    void processGetRequest(myStructures::processedThreadData processedRequest, QMap<QString, myStructures::processedThreadData> * processedRequestErrors = nullptr);
    void processMessageRequest(myStructures::processedThreadData processedRequest, QMap<QString, myStructures::processedThreadData> * processedRequestErrors = nullptr);
    void processConfirmRequest(myStructures::processedThreadData processedRequest, QMap<QString, myStructures::processedThreadData> * processedRequestErrors = nullptr);
    void processRequestRequest(myStructures::processedThreadData processedRequest, QMap<QString, myStructures::processedThreadData> * processedRequestErrors = nullptr);
    void processUpdateRequest(myStructures::processedThreadData processedRequest, QMap<QString, myStructures::processedThreadData> * processedRequestErrors = nullptr);
    void processErrors(QMap<QString, myStructures::processedThreadData> * errors);

signals:
    void newDataInBuffor();
    void replyReady(QByteArray processedRequestMd5Hash, QByteArray json);
//    void reply(quint64 size, QByteArray data);
    void exeDataBaseQuery(QString query, QString destDatabase, QList<QMap<QString,QVariant>> * results, QStringList * errors = nullptr);
    void newLog(QString log);
    void newLogToFile(QString sender, QString notes, QByteArray data);
    void processingErrors(QMap<QString, myStructures::processedThreadData> * errors);

};

#endif // C_ACTIONEXECUTIVE_H
