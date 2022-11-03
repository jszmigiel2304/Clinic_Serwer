#include "w_hostslistwindow.h"
#include "ui_w_hostslistwindow.h"


w_HostsListWindow &w_HostsListWindow::Instance()
{
    static w_HostsListWindow * _instance = nullptr;
    if ( _instance == nullptr ) {
        _instance = new w_HostsListWindow();
    }
    return *_instance;
}

w_HostsListWindow::w_HostsListWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::w_HostsListWindow)
{
    ui->setupUi(this);
    setWindowTitle(QApplication::applicationName());
    this->lay = new QVBoxLayout();
    this->actionGroup = new QActionGroup(this);
    connect(actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(disconnectHost(QAction*)));
}

w_HostsListWindow::w_HostsListWindow(const w_HostsListWindow &, QWidget *parent) :
    QMainWindow(parent)
{

}

void w_HostsListWindow::closeEvent(QCloseEvent *)
{
    this->setIsShown(false);
}

bool w_HostsListWindow::getIsShown() const
{
    return isShown;
}

void w_HostsListWindow::setIsShown(bool value)
{
    isShown = value;
}

w_HostsListWindow::~w_HostsListWindow()
{
    delete ui;
}


QMap<QString, QVariant> w_HostsListWindow::getServerProperties()
{
    QMap<QString, QVariant> map = this->watchedObjectsList["server"]->ShareProperties("hostsOnly");

    return map;
}

void w_HostsListWindow::update()
{

    foreach (QGroupBox * g, this->hostsGroupBoxList) {
        delete g;
    }
    this->hostsGroupBoxList.clear();

    delete this->lay;
    this->lay = new QVBoxLayout();

    ui->hostListScrollAreaContent->setLayout(lay);
    this->updateHostsList();

    ui->hostListScrollAreaContent->setGeometry(0,0,650, 50 * this->hosts.count() + 10);

    QListIterator<c_ClientConnection *> iterator(this->hosts);

    int index = 0;

    while(iterator.hasNext())
    {

        QGroupBox * gBox = new QGroupBox();
        gBox->setGeometry(0, 50 * index ,650, 40);
        gBox->setStyleSheet("margin: 0px 5px; padding: 0px");

        QLabel * ipLabel = new QLabel(gBox);
        ipLabel->setGeometry(0,0,150,40);
        ipLabel->setAlignment(Qt::AlignVCenter);
        ipLabel->setIndent(10);
        ipLabel->setStyleSheet("margin: 0px; padding: 0px");
        ipLabel->setText( iterator.peekNext()->getSocket()->peerAddress().toString() );

        QLabel * hostNameLabel = new QLabel(gBox);
        hostNameLabel->setGeometry(150,0,150,40);
        hostNameLabel->setAlignment(Qt::AlignVCenter);
        hostNameLabel->setIndent(10);
        ipLabel->setStyleSheet("margin: 0px; padding: 0px");
        hostNameLabel->setText( iterator.peekNext()->getSocket()->peerName() );

        QLabel * socketLabel = new QLabel(gBox);
        socketLabel->setGeometry(300,0,150,40);
        socketLabel->setAlignment(Qt::AlignVCenter);
        socketLabel->setIndent(10);
        ipLabel->setStyleSheet("margin: 0px; padding: 0px");
        socketLabel->setText( QString("%1").arg(iterator.peekNext()->getSocket()->socketDescriptor() ) );

        QPushButton * disconnectButton = new QPushButton(gBox);
        QPixmap pixmap(":/actions/icons/if_cross_48_10298.png");
        QIcon icon(pixmap);
        disconnectButton->setIcon(icon);
        disconnectButton->setIconSize(QSize(15,15));
        disconnectButton->setGeometry(455,5,30,30);
        ipLabel->setStyleSheet("margin: 0px; padding: 0px;");
        QAction * disconnectAction = new QAction();
        disconnectAction->setWhatsThis(QString("dcAct%1").arg(index));
        connect(disconnectButton, SIGNAL(clicked(bool)), disconnectAction, SLOT(trigger()));
        actionGroup->addAction(disconnectAction);


        this->hostsGroupBoxList.push_back(gBox);

        lay->addWidget(gBox);

        iterator.next();
        index ++;
    }

    QString tittle = QString("Lista podłączonych urządzeń: ") + QString("%1").arg(hosts.count());
    ui->label->setText(tittle);
}

void w_HostsListWindow::updateHostsList()
{
    this->hosts.clear();
    this->hosts = (dynamic_cast<c_ClinicTcpServer *>(this->watchedObjectsList["server"]))->getHostsList();
}


void w_HostsListWindow::on_refreshButton_clicked()
{
    this->update();
}

void w_HostsListWindow::disconnectHost(QAction *action)
{
    quint32 index;

    QString actionDesc = action->whatsThis();
    actionDesc.remove(0,5);

    index = actionDesc.toInt();

    (dynamic_cast<c_ClinicTcpServer *>(this->watchedObjectsList["server"]))->removeClient(index);

    this->update();
}
