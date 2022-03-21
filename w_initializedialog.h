#ifndef W_INITIALIZEDIALOG_H
#define W_INITIALIZEDIALOG_H

#include "c_settingscontroller.h"

#include <QDialog>

namespace Ui {
class w_initializeDialog;
}

class w_initializeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit w_initializeDialog(QApplication * app, c_SettingsController * settContr , QWidget *parent = 0);
    ~w_initializeDialog();
    bool checkFiles();


    QApplication * app;
    c_SettingsController * settContr;

    QString getCssFile() const;
    void setCssFile(const QString &value);

    QString getConfigFile() const;
    void setConfigFile(const QString &value);

private slots:
    void on_w_initializeDialog_finished(int result);

private:
    Ui::w_initializeDialog *ui;

    QString cssFile;
    QString configFile;

};

#endif // W_INITIALIZEDIALOG_H
