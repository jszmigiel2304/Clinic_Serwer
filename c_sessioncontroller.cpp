#include "c_sessioncontroller.h"

c_sessionController::c_sessionController()
{    
    logs = w_logsWindow::Instance();
}

c_sessionController::c_sessionController(c_MySqlDatabaseController *dataBasesCtrlr)
{
    setDataBasesCtrlr(dataBasesCtrlr);
    logs = w_logsWindow::Instance();
}

c_sessionController::~c_sessionController()
{
}

c_MySqlDatabaseController *c_sessionController::getDataBasesCtrlr() const
{
    return dataBasesCtrlr;
}

void c_sessionController::setDataBasesCtrlr(c_MySqlDatabaseController *newDataBasesCtrlr)
{
    dataBasesCtrlr = newDataBasesCtrlr;
}

QList<sessionInformations> c_sessionController::getSessionsFromDataBase(QString userName, QString userPassword)
{
    QList<sessionInformations> sessions;

    QString query("SELECT * FROM public.sessions LEFT JOIN public.users ON public.users.id = public.sessions.id_user WHERE public.users.name = 'jszmigiel2304' AND public.users.password = '\\xab02b2345238d001543b7ecf94273ad1' AND public.sessions.state = 0");

    QList<QVariantList> tempList = dataBasesCtrlr->exe(query, "Authorization");

    QString log = QString("Data received (Total rows fetched: %1) \n").arg(tempList.size());
    //logs->clearLogs();
    logs->addLog(log);


    return sessions;
}

sessionInformations c_sessionController::newSession(QString userName, QString userPassword)
{
    sessionInformations session;


    return session;
}
