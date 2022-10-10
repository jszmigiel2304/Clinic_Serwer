#ifndef W_LOGSWINDOW_H
#define W_LOGSWINDOW_H

#include <QMainWindow>
#include <QTextBrowser>
#include <QFile>
#include <QTextStream>
#include <QStringList>

namespace Ui {
class w_logsWindow;
}

class w_logsWindow : public QMainWindow
{
    Q_OBJECT

public:
    static w_logsWindow * Instance();
    ~w_logsWindow();

    QTextBrowser * getLogBrowser();
    void addSeparator(QChar sepChar = '#');
    void clearLogs();

public slots:
    void addLog(QString log);


private:
    explicit w_logsWindow(QWidget *parent = nullptr);
    Ui::w_logsWindow *ui;
};

#endif // W_LOGSWINDOW_H

