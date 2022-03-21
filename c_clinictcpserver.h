#ifndef C_CLINICTCPSERVER_H
#define C_CLINICTCPSERVER_H

#include "i_watched.h"
#include "c_mysqldatabasecontroller.h"
#include "w_runserverdialog.h"
#include "c_mydatabaseconnection.h"
#include "c_clientconnection.h"
#include "c_myconnection.h"
#include "w_logswindow.h"
#include "c_myparser.h"
#include "c_actionexecutive.h"
#include "c_logscontroller.h"
#include "c_structures.h"

#include <QTcpServer>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QMessageBox>
#include <QDataStream>
#include <QTextStream>
#include <QList>
#include <QMap>


class c_ClinicTcpServer : public QTcpServer, public i_Watched
{
    Q_OBJECT
public:
    c_ClinicTcpServer();
    c_ClinicTcpServer(QMap<QString, QVariant> settings, QObject *parent = nullptr);
    ~c_ClinicTcpServer();
    QMap<QString, QVariant> ShareProperties(QString sharedData = "all");
    QString ShareMessege();

    void runServer();
    void stopServer();
    void startServer();

    void setDbContr(c_MySqlDatabaseController *value);
    void newClient(c_ClientConnection *connection = nullptr);
    void removeClient(qintptr id);
    void removeClient(c_ClientConnection *connection = nullptr);
    void removeClients();

    QList<c_ClientConnection *> getHostsList() const;

    c_myParser *getParser() const;
    void setParser(c_myParser *value);

    c_actionExecutive *getExecutive() const;
    void setExecutive(c_actionExecutive *value);

    c_MySqlDatabaseController *getDbContr() const;

public slots:
    void UpdateProperties(QMap<QString, QVariant> map);
    void dataRead(quint32 size, QByteArray data);

signals:
    void PropertiesChanged();
    void MessageChanged(QString msg, int time);

private:
    quint16 port;
    QString iterfaceName;
    QHostAddress address;
    QString status;

    c_MySqlDatabaseController * dbContr;

    QList<c_ClientConnection *> hostsList;

    w_logsWindow * logs;

    c_myParser *parser;

    c_actionExecutive *executive;

protected:
    void incomingConnection(qintptr socketDescriptor);
};

#endif // C_CLINICTCPSERVER_H
