#include "w_runserverdialog.h"
#include "ui_w_runserverdialog.h"

w_runServerDialog::w_runServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_runServerDialog)
{
    ui->setupUi(this);

    QMovie *movie = new QMovie(":/gifs/gifs/ezgif.com-crop.gif");

    ui->processLabel->setMovie(movie);
    ui->processLabel->movie()->start();
}

w_runServerDialog::~w_runServerDialog()
{
    delete ui;
}

void w_runServerDialog::setProcessDescription()
{
    QString message = this->watchedObjectsList["server"]->ShareMessege();

    this->ui->processLabel->setText(message);
}
