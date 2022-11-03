#include "w_mainwindow.h"
#include "c_clinictcpserver.h"
#include "c_settingscontroller.h"
#include "w_initializedialog.h"
#include "c_mysqldatabasecontroller.h"
#include "c_logscontroller.h"
#include "c_myfiles.h"

#include <QApplication>
#include <QSettings>
#include <QFile>
#include <QSql>
#include <QStatusBar>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);    

    c_myFiles * files = c_myFiles::Instance();

    files->setCssFilePath(a.applicationDirPath() + "/styles.css");
    files->setConfigFilePath(a.applicationDirPath() + "/config.ini");

    c_SettingsController settContr(files->getConfigFilePath());

    a.setApplicationName("Clinic - Server");

    w_initializeDialog * init = new w_initializeDialog(&a, &settContr);
    init->setCssFile(files->getCssFilePath());
    init->setConfigFile(files->getConfigFilePath());
    init->show();

    if(!init->checkFiles())
    {
        files->deleteLater();
        a.exit(0);
    }
    else
    {
        w_MainWindow * w = new w_MainWindow(settContr.getSettings("window"));
        c_LogsController * logsCtrlr = c_LogsController::Instance();
        c_ClinicTcpServer * server = new c_ClinicTcpServer(settContr.getSettings("server"));

        server->setLogsContr(logsCtrlr);
        server->createDbContr(settContr.getSettings("databaseAuthentication"), settContr.getSettings("databaseClinic"));

        w->AddWatchedObject("server", dynamic_cast<i_Watched *> (server) );
        QObject::connect(server,
                SIGNAL(PropertiesChanged()),
                w,
                SLOT(refresh()), Qt::DirectConnection);
        w->AddWatchedObject("databaseController", dynamic_cast<i_Watched *> (server->getDbContr()));
        QObject::connect(server->getDbContr(),
                SIGNAL(PropertiesChanged()),
                w,
                SLOT(refresh()), Qt::DirectConnection);

        QObject::connect(server,
                         SIGNAL(MessageChanged(QString,int)),
                         w,
                         SLOT(changeStatusBarMessage(QString,int)), Qt::DirectConnection);

        w->shareServerPointer();
        w->shareDbContrPointer();

        w->MyShow();
        w->refresh();

        return a.exec();
    }        

    return a.exec();
}


