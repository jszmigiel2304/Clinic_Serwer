#include "w_databaseconnectionsconfigurationdialog.h"
#include "ui_w_databaseconnectionsconfigurationdialog.h"

w_DatabaseConnectionsConfigurationDialog::w_DatabaseConnectionsConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_DatabaseConnectionsConfigurationDialog)
{
    ui->setupUi(this);


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

w_DatabaseConnectionsConfigurationDialog::~w_DatabaseConnectionsConfigurationDialog()
{
    delete ui;
}

void w_DatabaseConnectionsConfigurationDialog::update()
{
    ui->AuthDbConnectionWidget->SetName("Authorization");
    ui->ClinicDbConnectionWidget->SetName("Clinic");

    ui->AuthDbConnectionWidget->update();
    ui->ClinicDbConnectionWidget->update();

    this->isChanged = false;
}

void w_DatabaseConnectionsConfigurationDialog::shareDbContrPointer()
{
    this->ShareWatchedObject("databaseController", dynamic_cast<i_Observer *>(ui->AuthDbConnectionWidget));
    this->ShareWatchedObject("databaseController", dynamic_cast<i_Observer *>(ui->ClinicDbConnectionWidget));
}

void w_DatabaseConnectionsConfigurationDialog::settingsChanged(bool)
{
    this->isChanged = true;
    (ui->buttonBox->buttons()[2])->setEnabled(true);
    (ui->buttonBox->buttons()[1])->setEnabled(true);

}

void w_DatabaseConnectionsConfigurationDialog::settingsChanged(QString)
{
    this->isChanged = true;
    (ui->buttonBox->buttons()[2])->setEnabled(true);
    (ui->buttonBox->buttons()[1])->setEnabled(true);

}

void w_DatabaseConnectionsConfigurationDialog::okButtonPressed()
{
    this->isChanged = false;
    emit(close());

}

void w_DatabaseConnectionsConfigurationDialog::resetButtonPressed()
{
    this->update();

    this->isChanged = false;
    (ui->buttonBox->buttons()[2])->setEnabled(false);
    (ui->buttonBox->buttons()[1])->setEnabled(false);

}

void w_DatabaseConnectionsConfigurationDialog::applyButtonPressed()
{
    //zapisz ustawienia


    QMap<QString, QVariant> mapAuth, mapClinic;

    mapAuth = ui->AuthDbConnectionWidget->getDbConnectionProperties();
    mapClinic = ui->ClinicDbConnectionWidget->getDbConnectionProperties();

    mapAuth.insert(mapClinic);  //nowe 6.2
    this->watchedObjectsList["databaseController"]->UpdateProperties(mapAuth); //nowe 6.2
//    this->watchedObjectsList["databaseController"]->UpdateProperties(mapAuth.unite(mapClinic));


    c_SettingsController settCtrl(c_myFiles::Instance()->getConfigFilePath());

    QMap<QString, QVariant> map;
    map.insert( "driver", mapAuth["authDriver"].toString() );
    map.insert( "address", mapAuth["authAddress"].toString() );
    map.insert( "port", mapAuth["authPort"].toString() );
    map.insert( "database_name", mapAuth["authDbName"].toString() );
    map.insert( "username", mapAuth["authUserName"].toString() );
    map.insert( "password", mapAuth["authPassword"].toString() );

    settCtrl.SaveToFile("databaseAuthentication", map);

    map.clear();

    map.insert( "driver", mapAuth["clinicDriver"].toString() );
    map.insert( "address", mapAuth["clinicAddress"].toString() );
    map.insert( "port", mapAuth["clinicPort"].toString() );
    map.insert( "database_name", mapAuth["clinicDbName"].toString() );
    map.insert( "username", mapAuth["clinicUserName"].toString() );
    map.insert( "password", mapAuth["clinicPassword"].toString() );

    settCtrl.SaveToFile("databaseClinic", map);

    this->isChanged = false;
    (ui->buttonBox->buttons()[2])->setEnabled(false);
    (ui->buttonBox->buttons()[1])->setEnabled(false);

}

void w_DatabaseConnectionsConfigurationDialog::closeEvent(QCloseEvent *e)
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
