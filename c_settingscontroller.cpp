#include "c_settingscontroller.h"

c_SettingsController::c_SettingsController()
{

}

c_SettingsController::c_SettingsController(QString settingsFilePath)
{
    this->settingsFilePath = settingsFilePath;
}

void c_SettingsController::LoadSettings()
{
        QSettings sett(this->settingsFilePath, QSettings::IniFormat);
        QStringList groups = sett.childGroups();

        foreach(QString groupName, groups)
        {
            sett.beginGroup(groupName);

            QMap <QString, QVariant> map;

            foreach (const QString &childKey, sett.childKeys())
            {
                map.insert(childKey, sett.value(childKey).toString());
            }

            this->settings.insert(groupName, map);

            sett.endGroup();
        }
}

void c_SettingsController::CreateIniFile()
{
    QSettings settings("/config.ini", QSettings::IniFormat);

    settings.beginGroup("application");
    settings.endGroup();

    settings.beginGroup("window");
    settings.setValue("start_minimize", WINDOW_START_MINIMIZE);
    settings.endGroup();

    settings.beginGroup("server");
    settings.setValue("interfaceName", SERVER_INTERFACE);
    settings.setValue("port", SERVER_PORT);
    settings.endGroup();

    settings.beginGroup("databaseAuthentication");
    settings.setValue("driver", DB_AUTH_DRIVER);
    settings.setValue("address", DB_AUTH_ADDRESS);
    settings.setValue("port", DB_AUTH_PORT);
    settings.setValue("database_name", DB_AUTH_NAME);
    settings.setValue("username", DB_AUTH_USER);
    settings.setValue("password", DB_AUTH_PWD);
    settings.endGroup();

    settings.beginGroup("databaseClinic");
    settings.setValue("driver", DB_CLINIC_DRIVER);
    settings.setValue("address", DB_CLINIC_ADDRESS);
    settings.setValue("port", DB_CLINIC_PORT);
    settings.setValue("database_name", DB_CLINIC_NAME);
    settings.setValue("username", DB_CLINIC_USER);
    settings.setValue("password", DB_CLINIC_PWD);
    settings.endGroup();

    settings.sync();
}

QString c_SettingsController::getSettingsFilePath() const
{
    return this->settingsFilePath;
}

QMap<QString, QMap<QString, QVariant>> c_SettingsController::getSettings() const
{
    return this->settings;
}

QMap<QString, QVariant> c_SettingsController::getSettings(QString groupName) const
{
    return this->settings[groupName];
}

void c_SettingsController::SaveToFile(QString groupName, QMap<QString, QVariant> map)
{

    QSettings settings(this->settingsFilePath, QSettings::IniFormat);

    settings.beginGroup(groupName);
    foreach (QString item, map.keys()) {
        settings.setValue(item, map[item]);
    }
    settings.endGroup();
}

void c_SettingsController::SaveToFile(QString groupName, QMap<QString, QVariant> map, QString filePath)
{
    QSettings settings(filePath, QSettings::IniFormat);

    settings.beginGroup(groupName);
    foreach (QString item, map.keys()) {
        settings.setValue(item, map[item]);
    }
    settings.endGroup();

}

