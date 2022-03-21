#include "w_mainwindow.h"
#include "c_clientconnection.h"
#include "c_clinictcpserver.h"
#include "c_settingscontroller.h"
#include "w_initializedialog.h"
#include "c_mysqldatabasecontroller.h"
#include "w_logswindow.h"
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

//    QString str = a.applicationDirPath();
//    QString strini = str + "/config.ini";
//    QString strcss = str + "/styles.css";

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
        c_ClinicTcpServer * server = new c_ClinicTcpServer(settContr.getSettings("server"));
        c_MySqlDatabaseController * dbContr = new c_MySqlDatabaseController(settContr.getSettings("databaseAuthentication"), settContr.getSettings("databaseClinic"));

        server->setDbContr(dbContr);

        w->AddWatchedObject("server", dynamic_cast<i_Watched *> (server) );
        QObject::connect(server,
                SIGNAL(PropertiesChanged()),
                w,
                SLOT(refresh()));
        w->AddWatchedObject("databaseController", dynamic_cast<i_Watched *> (dbContr));
        QObject::connect(dbContr,
                SIGNAL(PropertiesChanged()),
                w,
                SLOT(refresh()));

        QObject::connect(server,
                         SIGNAL(MessageChanged(QString,int)),
                         w,
                         SLOT(changeStatusBarMessage(QString,int))
                         );

        w->shareServerPointer();
        w->shareDbContrPointer();

        w_logsWindow::Instance()->show();
        QObject::connect(w, SIGNAL(destroyed(QObject *)), w_logsWindow::Instance(), SLOT(close()) );

        w->MyShow();
        w->refresh();


        return a.exec();

//        dbContr->deleteLater();
//        server->deleteLater();
//        w->deleteLater();
    }        

    return a.exec();
}


