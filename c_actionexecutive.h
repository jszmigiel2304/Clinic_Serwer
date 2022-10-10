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

    const QList<processedThreadData> &getReceivedDataFromClients() const;
    void setReceivedDataFromClients(const QList<processedThreadData> &newReceivedDataFromClients);

    bool getProcessingMode() const;
    void setProcessingMode(bool newProcessingMode);

    void processRequests(QMap<QString, processedThreadData> * processedRequestErrors = nullptr);

    c_MySqlDatabaseController *getDataBasesCtrlr() const;

public slots:
    void newDataReceivedFromClient(processedThreadData data);
    void setDataBasesCtrlr(c_MySqlDatabaseController *newDataBasesCtrlr);
    void dataReceived(processedThreadData data);

protected:

private:
    QList<processedThreadData> receivedDataFromClients;
    QMap<QString, processedThreadData> * processedRequestErrors;

    c_MySqlDatabaseController * dataBasesCtrlr;

    w_logsWindow *logs;

    bool processingMode;

private slots:
    void startProcessingRequests();
    void processGetRequest(processedThreadData processedRequest, QMap<QString, processedThreadData> * processedRequestErrors = nullptr);
    void processMessageRequest(processedThreadData processedRequest, QMap<QString, processedThreadData> * processedRequestErrors = nullptr);
    void processConfirmRequest(processedThreadData processedRequest, QMap<QString, processedThreadData> * processedRequestErrors = nullptr);
    void processRequestRequest(processedThreadData processedRequest, QMap<QString, processedThreadData> * processedRequestErrors = nullptr);
    void processUpdateRequest(processedThreadData processedRequest, QMap<QString, processedThreadData> * processedRequestErrors = nullptr);
    void processErrors(QMap<QString, processedThreadData> * errors);

signals:
    void newDataInBuffor();
    void replyReady(QByteArray processedRequestMd5Hash, QByteArray json);
//    void reply(quint64 size, QByteArray data);
    void exeDataBaseQuery(QString query, QString destDatabase, QList<QMap<QString,QVariant>> * results, QStringList * errors = nullptr);
    void newLog(QString log);
    void newLogToFile(QString sender, QString notes, QByteArray data);
    void processingErrors(QMap<QString, processedThreadData> * errors);

};

#endif // C_ACTIONEXECUTIVE_H
