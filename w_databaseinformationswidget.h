#ifndef W_DATABASEINFORMATIONSWIDGET_H
#define W_DATABASEINFORMATIONSWIDGET_H

#include "i_observer.h"

#include <QWidget>
#include <QTimer>

namespace Ui {
class w_DatabaseInformationsWidget;
}

class w_DatabaseInformationsWidget : public QWidget , public i_Observer
{
    Q_OBJECT

public:
    explicit w_DatabaseInformationsWidget(QWidget *parent = 0);
    ~w_DatabaseInformationsWidget();
    void setName(QString dbName);

public slots:
    void refresh();

private:
    Ui::w_DatabaseInformationsWidget *ui;
    QString dbName;
    QTimer * timer;
};

#endif // W_DATABASEINFORMATIONSWIDGET_H
