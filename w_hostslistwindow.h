#ifndef W_HOSTSLISTWINDOW_H
#define W_HOSTSLISTWINDOW_H


#include "i_observer.h"
#include "c_clinictcpserver.h"
#include "c_clientconnection.h"

#include <QDialog>
#include <QGroupBox>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QActionGroup>

namespace Ui {
class w_HostsListWindow;
}

class w_HostsListWindow : public QMainWindow, public i_Observer
{
    Q_OBJECT

public:
    static w_HostsListWindow & Instance();
    ~w_HostsListWindow();
    QMap<QString, QVariant> getServerProperties();
    void update();
    void updateHostsList();

    bool getIsShown() const;
    void setIsShown(bool value);

private slots:
    void on_refreshButton_clicked();
    void disconnectHost(QAction * action);

private:
    explicit w_HostsListWindow(QWidget *parent = nullptr);
    explicit w_HostsListWindow(const w_HostsListWindow& ,QWidget *parent = nullptr);
    void closeEvent(QCloseEvent * e);

    Ui::w_HostsListWindow *ui;
    QList<c_ClientConnection *> hosts;
    QList<QGroupBox *> hostsGroupBoxList;
    QVBoxLayout * lay;

    bool isShown;
    QActionGroup * actionGroup;
};

#endif // W_HOSTSLISTWINDOW_H
