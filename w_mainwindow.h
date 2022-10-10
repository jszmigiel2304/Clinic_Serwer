#ifndef W_MAINWINDOW_H
#define W_MAINWINDOW_H

#include "i_watched.h"
#include "i_observer.h"
#include "w_serverconfigurationdialog.h"
#include "c_clinictcpserver.h"
#include "c_mysqldatabasecontroller.h"
#include "w_windowconfigurationdialog.h"
#include "w_runserverdialog.h"
#include "w_databaseconnectionsconfigurationdialog.h"
#include "w_hostslistdialog.h"
#include "w_hostslistwindow.h"

#include <QObject>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QCloseEvent>

namespace Ui {
class w_MainWindow;
}

class w_MainWindow : public QMainWindow, public i_Watched, public i_Observer
{
    Q_OBJECT

public:
    explicit w_MainWindow(QWidget *parent = nullptr);
    explicit w_MainWindow(QMap<QString, QVariant> settings, QWidget *parent = nullptr);
    ~w_MainWindow();
    QMap<QString, QVariant> ShareProperties(QString sharedData = "all");
    void UpdateProperties(QMap<QString, QVariant> map);

    void showEvent(QShowEvent *);

    void shareServerPointer();
    void shareDbContrPointer();

    void MyShow();

    QStatusBar GetStatusBar();

public slots:
    void refresh();
    void changeStatusBarMessage(QString msg, int time);

private slots:
    void on_actionServerConfigure_triggered();

    void on_actionServerStart_triggered();

    void on_actionServerStop_triggered();

    void on_actionWindowConfigure_triggered();

    void on_actionOpen_triggered();

    void ontrayIconactivated(QSystemTrayIcon::ActivationReason reason);

    void on_actionClose_triggered();

    void on_actionDataBaseConfigure_triggered();

    void on_actionHostsList_triggered();

private:
    void createTrayIcon();
    void closeEvent(QCloseEvent * event);

    Ui::w_MainWindow *ui;
    bool startMinimize;
    bool minimizeToTrayOnClose;

    QSystemTrayIcon * trayIcon;
    QMenu * trayIconMenu;

    w_HostsListWindow * hostsListWindow;

signals:
    void PropertiesChanged();
};

#endif // W_MAINWINDOW_H
