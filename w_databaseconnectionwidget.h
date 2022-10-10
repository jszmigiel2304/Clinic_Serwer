#ifndef W_DATABASECONNECTIONWIDGET_H
#define W_DATABASECONNECTIONWIDGET_H

#include "i_observer.h"
#include "c_mydatabaseconnection.h"
#include "m_databasedrivers.h"
#include "w_databaseconnectionsconfigurationdialog.h"

#include <QWidget>
#include <QTcpSocket>
#include <QTimer>

namespace Ui {
class w_DatabaseConnectionWidget;
}

class w_DatabaseConnectionWidget : public QWidget, public  i_Observer
{
    Q_OBJECT

public:
    explicit w_DatabaseConnectionWidget(QWidget *parent = 0);
    ~w_DatabaseConnectionWidget();
    void update();
    QMap<QString, QVariant> getDbControllerProperties();
    QMap<QString, QVariant> getDbConnectionProperties();
    void SetName(QString name);

private slots:
    void on_testConnectionButton_clicked();
    void RefreshButtonOTimeout();
    void TestStateChanged(QAbstractSocket::SocketState state);

private:
    Ui::w_DatabaseConnectionWidget *ui;
    QString dbName;

signals:
};

#endif // W_DATABASECONNECTIONWIDGET_H
