#include "w_initializedialog.h"
#include "ui_w_initializedialog.h"
#include <QMovie>
#include <QPushButton>


w_initializeDialog::w_initializeDialog(QApplication * app, c_SettingsController * settContr ,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_initializeDialog)
{
    ui->setupUi(this);

    this->app = app;
    this->settContr = settContr;


    QMovie *movie = new QMovie(":/gifs/gifs/ezgif.com-crop.gif");

    ui->processLabel->setMovie(movie);
    ui->processLabel->movie()->start();


}

w_initializeDialog::~w_initializeDialog()
{
    delete ui;
}

bool w_initializeDialog::checkFiles()
{
    QFile iniFfile(this->configFile);

    if(!iniFfile.exists())
    {

        ui->describeLabel->setText("Nie znaleziono pliku: config.ini");

        // utwórz plikconfig.ini

        ui->describeLabel->setText("Tworzenie pliku: config.ini");
        this->settContr->CreateIniFile();
        this->settContr->LoadSettings();
    }
    else
    {
        ui->describeLabel->setText("Wczytuję ustawienia z pliku: config.ini");
        this->settContr->LoadSettings();
    }

    QFile cssFile(this->cssFile);

    if(!cssFile.open(QFile::ReadOnly))
    {
        ui->describeLabel->setText("Nie znaleziono pliku: styles.css");

        QPixmap img(":/actions/icons/error80x80.png");
        ui->processLabel->setPixmap(img);

        QPushButton * button = new QPushButton(this);
        button->setGeometry(90,120,70,30);
        button->setText("Zakończ");
        button->setStyleSheet("background-color: #fafafb; border: 1px solid black;");

        connect(button, SIGNAL( pressed() ), this, SLOT( close() ) );
        button->show();

        this->exec();

        return false;
    } else
    {
        QString cssStyles = QLatin1String(cssFile.readAll());
        this->app->setStyleSheet(cssStyles);
    }

    this->close();

    return true;
}

void w_initializeDialog::on_w_initializeDialog_finished(int)
{

}

QString w_initializeDialog::getConfigFile() const
{
    return configFile;
}

void w_initializeDialog::setConfigFile(const QString &value)
{
    configFile = value;
}

QString w_initializeDialog::getCssFile() const
{
    return cssFile;
}

void w_initializeDialog::setCssFile(const QString &value)
{
    cssFile = value;
}
