#ifndef W_INTERFACEINFORMATIONSWIDGET_H
#define W_INTERFACEINFORMATIONSWIDGET_H

#include "i_observer.h"
#include "i_watched.h"

#include <QWidget>
#include <QNetworkInterface>
#include <QTimer>

namespace Ui {
class w_InterfaceInformationsWidget;
}

class w_InterfaceInformationsWidget : public QWidget , public i_Observer
{
    Q_OBJECT

public:
    explicit w_InterfaceInformationsWidget(QWidget *parent = 0);
    ~w_InterfaceInformationsWidget();

public slots:
    //void refreshInformations(QMap<QString, QVariant> map);
    void refresh();


private:
    Ui::w_InterfaceInformationsWidget *ui;
    QTimer * timer;
};

#endif // W_INTERFACEINFORMATIONSWIDGET_H
