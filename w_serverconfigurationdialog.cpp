#include "w_serverconfigurationdialog.h"
#include "ui_w_serverconfigurationdialog.h"


w_serverConfigurationDialog::w_serverConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_serverConfigurationDialog)
{
    ui->setupUi(this);




    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    foreach (QNetworkInterface interface, interfaces) {
        QRadioButton * radio = new QRadioButton(ui->interfaceListGroupBox);
        radio->setText(interface.name());
        radio->setGeometry(10, (20 + interfacesRadioButtonList.length() * 20), 350, 20);        
        connect(radio, SIGNAL(clicked(bool)), this, SLOT(settingsChanged(bool)));
        this->interfacesRadioButtonList.push_back(radio);

    }

    connect(ui->interfaceRadioButton, SIGNAL(clicked(bool)), this, SLOT(listeningOptionGroupBoxClicked(bool)));
    connect(ui->allRadioButton, SIGNAL(clicked(bool)), this, SLOT(listeningOptionGroupBoxClicked(bool)));

    connect(ui->allRadioButton, SIGNAL(clicked(bool)), this, SLOT(settingsChanged(bool)));
    connect(ui->interfaceRadioButton, SIGNAL(clicked(bool)), this, SLOT(settingsChanged(bool)));
    connect(ui->allRadioButton, SIGNAL(clicked(bool)), this, SLOT(settingsChanged(bool)));

    ui->portLineEdit->setValidator(new QIntValidator(0, 65535, this));
    connect(ui->portLineEdit, SIGNAL(textEdited(QString)),this, SLOT(settingsChanged(QString)));

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

w_serverConfigurationDialog::~w_serverConfigurationDialog()
{
    delete ui;
}

QMap<QString, QVariant> w_serverConfigurationDialog::getServerProperties()
{
    QMap<QString, QVariant> map = this->watchedObjectsList["server"]->ShareProperties("basicOnly");

    return map;
}

void w_serverConfigurationDialog::update()
{
    QMap<QString, QVariant> map = this->getServerProperties();

    ui->portLineEdit->setText(map["port"].toString());

    if(map["interfaceName"].toString() == "any")
    {
        ui->interfaceListGroupBox->setEnabled(false);
        ui->allRadioButton->setChecked(true);
    } else
    {
        ui->interfaceListGroupBox->setEnabled(true);
        ui->interfaceRadioButton->setChecked(true);
        foreach (QRadioButton * button, this->interfacesRadioButtonList) {
            if (button->text() == map["interfaceName"].toString())
            {
                button->setChecked(true);
                break;
            }
        }


    }
    this->isChanged = false;
}

void w_serverConfigurationDialog::listeningOptionGroupBoxClicked(bool)
{
    if(ui->interfaceRadioButton->isChecked())
    {
        ui->interfaceListGroupBox->setEnabled(true);
        this->interfacesRadioButtonList[0]->setChecked(true);
    }
    else
        ui->interfaceListGroupBox->setEnabled(false);
}

void w_serverConfigurationDialog::settingsChanged(bool)
{
    this->isChanged = true;
    (ui->buttonBox->buttons()[2])->setEnabled(true);
    (ui->buttonBox->buttons()[1])->setEnabled(true);
}

void w_serverConfigurationDialog::settingsChanged(QString)
{
    this->isChanged = true;
    (ui->buttonBox->buttons()[2])->setEnabled(true);
    (ui->buttonBox->buttons()[1])->setEnabled(true);
}

void w_serverConfigurationDialog::okButtonPressed()
{
    this->isChanged = false;
    emit(close());
}

void w_serverConfigurationDialog::resetButtonPressed()
{
    this->update();

    this->isChanged = false;
    (ui->buttonBox->buttons()[2])->setEnabled(false);
    (ui->buttonBox->buttons()[1])->setEnabled(false);
}

void w_serverConfigurationDialog::applyButtonPressed()
{
    //zapisz ustawienia


    QMap<QString, QVariant> map;
    map.insert("port", ui->portLineEdit->text());

    if (ui->allRadioButton->isChecked())
    {
        map.insert("interfaceName", "any");
    }
    else
    {
        foreach (QRadioButton * button, this->interfacesRadioButtonList) {
            if (button->isChecked())
            {
                map.insert("interfaceName", button->text());
                break;
            }
        }
    }

    this->watchedObjectsList["server"]->UpdateProperties(map);

    c_SettingsController settCtrl(c_myFiles::Instance()->getConfigFilePath());
    settCtrl.SaveToFile("server", map);

    this->isChanged = false;
    (ui->buttonBox->buttons()[2])->setEnabled(false);
    (ui->buttonBox->buttons()[1])->setEnabled(false);
}

void w_serverConfigurationDialog::closeEvent(QCloseEvent *e)
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
