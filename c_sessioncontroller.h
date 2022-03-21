#ifndef C_SESSIONCONTROLLER_H
#define C_SESSIONCONTROLLER_H

#include "c_mysqldatabasecontroller.h"
#include "c_structures.h"
#include "w_logswindow.h"

#include <QObject>
#include <QList>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlResult>

class c_sessionController : QObject
{
    Q_OBJECT

public:
    c_sessionController();
    c_sessionController(c_MySqlDatabaseController * dataBasesCtrlr = Q_NULLPTR);
    ~c_sessionController();

    c_MySqlDatabaseController *getDataBasesCtrlr() const;
    void setDataBasesCtrlr(c_MySqlDatabaseController *newDataBasesCtrlr);

    QList<sessionInformations> getSessionsFromDataBase(QString userName, QString userPassword);
    sessionInformations newSession(QString userName, QString userPassword);

private:
    c_MySqlDatabaseController * dataBasesCtrlr;

    w_logsWindow * logs;


};

#endif // C_SESSIONCONTROLLER_H
