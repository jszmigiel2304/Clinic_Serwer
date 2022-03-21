#include "w_windowconfigurationdialog.h"
#include "ui_w_windowconfigurationdialog.h"

w_windowConfigurationDialog::w_windowConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_windowConfigurationDialog)
{
    ui->setupUi(this);

    connect(ui->minimizeToTrayOnCloseCheckBox,
            SIGNAL(clicked(bool)),
            this,
            SLOT(settingsChanged(bool)));

    connect(ui->startMinimizeCheckBox,
            SIGNAL(clicked(bool)),
            this,
            SLOT(settingsChanged(bool)));

    QPushButton * okButton = new QPushButton();
    QPushButton * resetButton = new QPushButton();
    QPushButton * applyButton = new QPushButton();

    okButton->setText("Ok");
    resetButton->setText("Reset");
    applyButton->setText("Zastosuj");


    resetButton->setEnabled(false);
    applyButton->setEnabled(false);

    okButton->setDefault(true);

    connect(okButton, SIGNAL(pressed()), this, SLOT(okButtonPressed()));
    connect(resetButton, SIGNAL(pressed()), this, SLOT(resetButtonPressed()));
    connect(applyButton, SIGNAL(pressed()), this, SLOT(applyButtonPressed()));

    ui->buttonBox->addButton(okButton, QDialogButtonBox::ActionRole);
    ui->buttonBox->addButton(resetButton, QDialogButtonBox::ActionRole);
    ui->buttonBox->addButton(applyButton, QDialogButtonBox::ActionRole);

}

w_windowConfigurationDialog::~w_windowConfigurationDialog()
{
    delete ui;
}

QMap<QString, QVariant> w_windowConfigurationDialog::getWindowProperties()
{
    QMap<QString, QVariant> map = this->watchedObjectsList["appWindow"]->ShareProperties("basicOnly");

    return map;
}

void w_windowConfigurationDialog::update()
{
    QMap<QString, QVariant> map = this->getWindowProperties();

    if(map["startMinimize"].toBool())
        ui->startMinimizeCheckBox->setChecked(true);
    else
        ui->startMinimizeCheckBox->setChecked(false);

    if(map["minimizeToTrayOnClose"].toBool())
        ui->minimizeToTrayOnCloseCheckBox->setChecked(true);
    else
        ui->minimizeToTrayOnCloseCheckBox->setChecked(false);

    this->isChanged = false;
}

void w_windowConfigurationDialog::settingsChanged(bool)
{
    this->isChanged = true;
    (ui->buttonBox->buttons()[2])->setEnabled(true);
    (ui->buttonBox->buttons()[1])->setEnabled(true);
}

void w_windowConfigurationDialog::settingsChanged(QString)
{
    this->isChanged = true;
    (ui->buttonBox->buttons()[2])->setEnabled(true);
    (ui->buttonBox->buttons()[1])->setEnabled(true);
}

void w_windowConfigurationDialog::okButtonPressed()
{
    this->isChanged = false;
    emit(close());
}

void w_windowConfigurationDialog::resetButtonPressed()
{
    this->update();

    this->isChanged = false;
    (ui->buttonBox->buttons()[2])->setEnabled(false);
    (ui->buttonBox->buttons()[1])->setEnabled(false);
}

void w_windowConfigurationDialog::applyButtonPressed()
{
    //zapisz ustawienia


    QMap<QString, QVariant> map;

    map.insert("startMinimize", ui->startMinimizeCheckBox->isChecked());
    map.insert("minimizeToTrayOnClose", ui->minimizeToTrayOnCloseCheckBox->isChecked());

    this->watchedObjectsList["appWindow"]->UpdateProperties(map);

    c_SettingsController settCtrl(c_myFiles::Instance()->getConfigFilePath());
    settCtrl.SaveToFile("window", map);

    this->isChanged = false;
    (ui->buttonBox->buttons()[2])->setEnabled(false);
    (ui->buttonBox->buttons()[1])->setEnabled(false);

}

void w_windowConfigurationDialog::closeEvent(QCloseEvent *e)
{
    if(this->isChanged)
    {
        QMessageBox::StandardButton close;
          close = QMessageBox::question(this, "Zakończ", "Ustawienia zostały zmienione. Zapisać ?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (close == QMessageBox::Yes) {
            this->applyButtonPressed();
              e->accept();
          } else {
            e->accept();
          }
    } else
    {
        e->accept();
    }


}
