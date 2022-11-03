#ifndef W_SERVERINFORMATIONSWIDGET_H
#define W_SERVERINFORMATIONSWIDGET_H

#include "i_observer.h"

#include <QWidget>
#include <QMap>
#include <QTimer>

namespace Ui {
class w_ServerInformationsWidget;
}

class w_ServerInformationsWidget : public QWidget , public i_Observer
{
    Q_OBJECT

public:
    explicit w_ServerInformationsWidget(QWidget *parent = 0);
    ~w_ServerInformationsWidget();

public slots:
    void refresh();
private:
    Ui::w_ServerInformationsWidget *ui;
    QTimer * timer;
};

#endif // W_SERVERINFORMATIONSWIDGET_H
