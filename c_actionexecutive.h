#ifndef C_ACTIONEXECUTIVE_H
#define C_ACTIONEXECUTIVE_H

#include "c_clientconnection.h"
#include "w_logswindow.h"
#include "c_structures.h"
#include "c_sessioncontroller.h"


#include <QObject>
#include <QAuthenticator>
#include <QMap>
#include <QList>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlResult>

class c_actionExecutive : public QObject
{
    Q_OBJECT
public:
    explicit c_actionExecutive(QObject *parent = nullptr);
    ~c_actionExecutive();

    void newDataReceivedFromClient(threadData data);

    w_logsWindow *getLogs() const;
    void setLogs(w_logsWindow *value);

    const QList<threadData> &getReceivedDataFromClients() const;
    void setReceivedDataFromClients(const QList<threadData> &newReceivedDataFromClients);

    bool getProcessingMode() const;
    void setProcessingMode(bool newProcessingMode);

    void processRequests(QMap<threadData, QString> * processedRequestErrors = Q_NULLPTR);


public slots:

protected:
    void log_in(QAuthenticator auth, QByteArray * answer);

private:
    QList<threadData> receivedDataFromClients;
    QMap<threadData, QString> * processedRequestErrors;

    w_logsWindow *logs;

    bool processingMode;

private slots:
    void startProcessingRequests();
    void processGetRequest(threadData processedRequest, QMap<threadData, QString> * processedRequestErrors = Q_NULLPTR);

signals:
    void newDataInBuffor();

};

#endif // C_ACTIONEXECUTIVE_H
