#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

#include <QString>
#include <QMessageBox>
#include <QTcpSocket>
#include <QPainter>
#include <QBuffer>
#include "qsensehatfb.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),ipValide(false),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->labelMasque->setText(" ");
    interfaceActive=QNetworkInterface::interfaceFromName("eth0");
    ui->groupBoxCablage->setFocus();

    //QNetworkInterface *inter = new QNetworkInterface();
    QList<QNetworkAddressEntry> list;
    //ipOrigine=interfaceActive.allAddresses();
    list=interfaceActive.addressEntries();
    qDebug()<<list.at(0).ip().toString();
    qDebug()<<list.at(0).netmask().toString();
    qDebug()<<list.at(0).prefixLength();
    ipOrigine=list.at(0);
    // effacer senseHat
    QSenseHatFb fb;
    if (!fb.isValid())
    {
        qDebug()<<"pas acces fb de sensehat";
    }
    else
    {

        fb.setLowLight(true);
        QPainter p(fb.paintDevice());
        Qt::GlobalColor couleur=Qt::red;
        p.setPen(couleur);
        p.fillRect(QRect(QPoint(),fb.size()),Qt::black);
    }
    // lancement timer detection branchement
    timerEtatInterface = new QTimer(this);
    connect(timerEtatInterface, SIGNAL(timeout()), this, SLOT(onQTimerIsRunning()));
    timerEtatInterface->start(2000);
    majGroupBox(false,false,false);
    QPalette p;
    p = palette();
    p.setBrush(QPalette::Window, QBrush(QPixmap(":/system.png")));
    ui->groupBox_3->setPalette(p);
}

Widget::Widget(QString coul, QWidget *parent):ipValide(false),QWidget(parent),sock(NULL),sockClientServ(NULL),sockServ(NULL),ipFixee(false),
  ui(new Ui::Widget)
{
    ui->setupUi(this);
    // assignation de la couleur du raspi
    if(coul=="j"){
        couleurRaspi=Qt::yellow;
    }
    if(coul=="v"){
        couleurRaspi=Qt::green;
    }
    if(coul=="b"){
        couleurRaspi=Qt::cyan;
    }
    if(coul=="m"){
        couleurRaspi=Qt::magenta;
    }
    QString css="background-color : rgb("+QString::number(couleurRaspi.red())+","+QString::number(couleurRaspi.green())+","+QString::number(couleurRaspi.blue())+")";
    ui->groupBoxCablage->setStyleSheet(css);
    interfaceActive=QNetworkInterface::interfaceFromName("eth0");

    //QNetworkInterface *inter = new QNetworkInterface();
    QList<QNetworkAddressEntry> list;
    //ipOrigine=interfaceActive.allAddresses();
    list=interfaceActive.addressEntries();
    qDebug()<<list.at(0).ip().toString();
    qDebug()<<list.at(0).netmask().toString();
    qDebug()<<list.at(0).prefixLength();
    ipOrigine=list.at(0);
    // effacer senseHat
    eteindreLeds();
    // lancement timer detection branchement
    timerEtatInterface = new QTimer(this);
    connect(timerEtatInterface, SIGNAL(timeout()), this, SLOT(onQTimerIsRunning()));
    timerEtatInterface->start(2000);
    majGroupBox(false,false,false);
    ui->spinBoxOctet1->setStyleSheet("QSpinBox{background-color : white};");
    ui->spinBoxOctet2->setStyleSheet("QSpinBox{background-color : white};");
    ui->spinBoxOctet3->setStyleSheet("QSpinBox{background-color : white};");
}

Widget::~Widget()
{
    reInitIp();
    eteindreLeds();
    delete ui;
}

void Widget::eteindreLeds()
{
    // eteindre leds
    QSenseHatFb fb;
    if (!fb.isValid())
    {
        qDebug()<<"pas acces fb de sensehat";
    }
    else
    {

        fb.setLowLight(true);
        QPainter p(fb.paintDevice());
        Qt::GlobalColor couleur=Qt::red;
        p.setPen(couleur);
        p.fillRect(QRect(QPoint(),fb.size()),Qt::black);
    }
}

void Widget::allumerSmiley(bool happy, QColor couleur)
{
    eteindreLeds();
    QSenseHatFb fb;
    if (!fb.isValid())
    {
        qDebug()<<"pas acces fb de sensehat";
    }
    else
    {

        fb.setLowLight(true);
        QPainter p(fb.paintDevice());
        p.setPen(couleur);
        // joyeux
        if (happy)
        {
            /*
             01234567
            0xxxxxxxx
            1xxxxxxxx
            2xxOxxOxx
            3xxxxxxxx
            4xOxxxxOx
            5xxOOOOxx
            6xxxxxxxx
            7xxxxxxxx

            */
                    p.drawPoint(2,2);
                    p.drawPoint(5,2);
                    p.drawPoint(1,4);
                    p.drawPoint(6,4);
                    p.drawPoint(2,5);
                    p.drawPoint(3,5);
                    p.drawPoint(4,5);
                    p.drawPoint(5,5);

        }
        else        // triste
        {
            p.drawPoint(2,2);
            p.drawPoint(5,2);
            p.drawPoint(2,4);
            p.drawPoint(3,4);
            p.drawPoint(4,4);
            p.drawPoint(5,4);
            p.drawPoint(1,5);
            p.drawPoint(6,5);
        }

    }
}

void Widget::on_pushButtonEtat_clicked()
{

    reInitIp();

}

void Widget::on_pushButtonAdresse_clicked()
{

    eteindreLeds();
}

/**
 * @brief test si interface est branche
 * si oui allumer en orange
 * sinon allumer en rouge
 */
void Widget::onQTimerIsRunning()
{
    interfaceActive=QNetworkInterface::interfaceFromName("eth0");
    if (interfaceActive.isValid())
    {
       /*ui->textEdit->append(interfaceActive.humanReadableName());
        if (interfaceActive.flags() & QNetworkInterface::IsUp )
        {
            ui->textEdit->append("interface up");

        }
        else
        {
            ui->textEdit->append("interface down");
        }*/
        // cable branche
        if (interfaceActive.flags() & QNetworkInterface::IsRunning )
        {
           // ui->textEdit->append("interface running");
            // pas d'adresse ip donne par l'utilisateur
            if (!ipFixee){
                majGroupBox(true,true,false);
                // allume orange
                allumerEtat( QColor("#FF9400")); // Orange
                ui->spinBoxOctet1->setStyleSheet("QSpinBox{background-color : white};");
                ui->spinBoxOctet2->setStyleSheet("QSpinBox{background-color : white};");
                ui->spinBoxOctet3->setStyleSheet("QSpinBox{background-color : white};");
                ui->spinBoxOctet1Contact->setStyleSheet("QSpinBox{background-color : white};");
                ui->spinBoxOctet2Contact->setStyleSheet("QSpinBox{background-color : white};");
                ui->spinBoxOctet3Contact->setStyleSheet("QSpinBox{background-color : white};");
            }
            else    // il y a une adresse ip donnee par l'utilisateur
            {
                majGroupBox(true,false,true);
                allumerEtat(Qt::green);
            }

        }
        else   // cable debranche
        {
           // ui->textEdit->append("interface not running");
            reInitIp();    // remettre l'adresse ip initiales et supprimer les autres
            majGroupBox(false,false,false);
            // allume rouge
            eteindreLeds();
            allumerEtat(Qt::red);
            ipFixee=false;
            ui->spinBoxOctet1->setStyleSheet("QSpinBox{background-color : white};");
            ui->spinBoxOctet2->setStyleSheet("QSpinBox{background-color : white};");
            ui->spinBoxOctet3->setStyleSheet("QSpinBox{background-color : white};");
            ui->spinBoxOctet1Contact->setStyleSheet("QSpinBox{background-color : white};");
            ui->spinBoxOctet2Contact->setStyleSheet("QSpinBox{background-color : white};");
            ui->spinBoxOctet3Contact->setStyleSheet("QSpinBox{background-color : white};");

        }

    }

}

void Widget::majGroupBox(bool cable, bool ip, bool ping)
{
    if (cable)
    {
        ui->labelBranchement->setText("Le câble ethernet est branché");
        ui->labelConnexion->show();
        ui->labelDisConnexion->hide();

    }
    else
    {
        ui->labelBranchement->setText("Branchez le câble ethernet");
        ui->labelConnexion->hide();
        ui->labelDisConnexion->show();
    }
    ui->groupBoxSetIp->setEnabled(ip);
    ui->groupBox_3->setEnabled(ping);
}

bool Widget::verifIp()
{
  //  int octet[4];
    bool valide=false;
    interfaceActive=QNetworkInterface::interfaceFromName("eth0");
    if (interfaceActive.allAddresses().count()>1)
    {
        valide=true;
    }
 /*   octet[0]=ui->spinBoxOctet1->value();
    octet[1]=ui->spinBoxOctet2->value();
    octet[2]=ui->spinBoxOctet3->value();
    octet[3]=ui->spinBoxOctet4->value();
    if (!((octet[0]>=1) && (octet[0]<=223)))
    {
        valide=false;
    }
    for (int i=1;i<2;i++)
    {
        if (!((octet[i]>=0) && (octet[i]<=255)))
        {
            valide=false;
        }
    }
    if (!((octet[3]>=1) && (octet[3]<=254)))
    {
        valide=false;
    }*/
    return valide;

}

void Widget::reInitIp()
{
    // suppression et reinit des sockets
    if (sock!=NULL){
        sock->deleteLater();
        qApp->processEvents();
        sock=NULL;
    }
    if (sockClientServ!=NULL){
        sockClientServ->deleteLater();
        qApp->processEvents();
        sockClientServ=NULL;
    }
    if (sockServ!=NULL){
        sockServ->deleteLater();
        qApp->processEvents();
        sockServ=NULL;
    }
    // remettre ip par defaut sur l'interface visuelle
    ui->spinBoxOctet1->setValue(1);
    ui->spinBoxOctet2->setValue(0);
    ui->spinBoxOctet3->setValue(0);
    ui->spinBoxOctet4->setValue(1);




    // lister les adresses ip pour eth0
    interfaceActive=QNetworkInterface::interfaceFromName("eth0");

    //QNetworkInterface *inter = new QNetworkInterface();
    QList<QNetworkAddressEntry> list;
    //ipOrigine=interfaceActive.allAddresses();
    list=interfaceActive.addressEntries();
    foreach (QNetworkAddressEntry currentAdr, list) {
        if (currentAdr!=ipOrigine){
            QString program = "ip";
            QStringList arguments;

            QString adresse=currentAdr.ip().toString()+"/"+QString::number(currentAdr.prefixLength());
            arguments << "addr" << "del"<<adresse<<"dev"<<"eth0";
            qDebug()<<"argument de la commande ip : "<<arguments;
            QProcess *myProcess = new QProcess(this);
            connect(myProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(onQProcessFinishedResetIp(int,QProcess::ExitStatus)));
            myProcess->start(program, arguments);
        }
    }
}

void Widget::allumerEtat(QColor couleurLed)
{
    QSenseHatFb fb;
    if (!fb.isValid())
    {
        qDebug()<<"pas acces fb de sensehat";
    }
    else
    {
        fb.setLowLight(true);
        QPainter p(fb.paintDevice());
      //  p.setPen(couleurRaspi);
        //p.drawRect(QRect(0,0,fb.size().width()-1,fb.size().height()-1));
        p.setPen(couleurLed);
        p.drawPoint(0,0);
        p.drawPoint(0,7);
        p.drawPoint(7,0);
        p.drawPoint(7,7);

        //p.fillRect(QRect(QPoint(3,3),/*fb.size()*/QSize(2,2)),couleurLed);

    }

}

/**
 * @brief tester la validite de du reseau et du numero d'hote
 * si ok fixer adresse et autorise ping et desactiver bloc de saisie d'adresse allumer en vert
 */
void Widget::on_pushButtonSetIp_clicked()
{

    QString program = "ip";
    QStringList arguments;
    ui->spinBoxOctet1Contact->setValue(1);
    ui->spinBoxOctet2Contact->setValue(0);
    ui->spinBoxOctet3Contact->setValue(0);
    ui->spinBoxOctet4Contact->setValue(1);
    if (verifIp())
    {

        quint8 prefix,octet1=ui->spinBoxOctet1->value();
        if (octet1>=1 && octet1<=127)
        {
            prefix=8;
            ui->spinBoxOctet1->setStyleSheet("QSpinBox{background-color : #F4C542};");
            ui->spinBoxOctet2->setStyleSheet("QSpinBox{background-color : white};");
            ui->spinBoxOctet3->setStyleSheet("QSpinBox{background-color : white};");
            ui->spinBoxOctet1Contact->setStyleSheet("QSpinBox{background-color : #F4C542};");
            ui->spinBoxOctet2Contact->setStyleSheet("QSpinBox{background-color : white};");
            ui->spinBoxOctet3Contact->setStyleSheet("QSpinBox{background-color : white};");

        }
        if (octet1>=128 && octet1<=191)
        {
            prefix=16;
             ui->spinBoxOctet1->setStyleSheet("QSpinBox{background-color : #F4C542};");
             ui->spinBoxOctet2->setStyleSheet("QSpinBox{background-color : #F4C542};");
             ui->spinBoxOctet3->setStyleSheet("QSpinBox{background-color : #white};");
             ui->spinBoxOctet1Contact->setStyleSheet("QSpinBox{background-color : #F4C542};");
             ui->spinBoxOctet2Contact->setStyleSheet("QSpinBox{background-color : #F4C542};");
             ui->spinBoxOctet3Contact->setStyleSheet("QSpinBox{background-color : #white};");
        }
        if (octet1>=192 && octet1<=223)
        {
            prefix=24;
            ui->spinBoxOctet1->setStyleSheet("QSpinBox{background-color : #F4C542};");
            ui->spinBoxOctet2->setStyleSheet("QSpinBox{background-color : #F4C542};");
            ui->spinBoxOctet3->setStyleSheet("QSpinBox{background-color : #F4C542};");
            ui->spinBoxOctet1Contact->setStyleSheet("QSpinBox{background-color : #F4C542};");
            ui->spinBoxOctet2Contact->setStyleSheet("QSpinBox{background-color : #F4C542};");
            ui->spinBoxOctet3Contact->setStyleSheet("QSpinBox{background-color : #F4C542};");
        }
        QString adresse=ui->spinBoxOctet1->text()+"."+ui->spinBoxOctet2->text()+"."+ui->spinBoxOctet3->text()+"."+ui->spinBoxOctet4->text()+"/"+QString::number(prefix);
        arguments << "addr" << "add"<<adresse<<"dev"<<"eth0";
        qDebug()<<"argument de la commande ip : "<<arguments;
        QProcess *myProcess = new QProcess(this);
        connect(myProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(onQProcessFinished(int,QProcess::ExitStatus)));
        myProcess->start(program, arguments);
        QNetworkAddressEntry source;
        adresse = adresse.left(adresse.lastIndexOf("/"));
        source.setIp(QHostAddress(adresse));
        source.setPrefixLength(prefix);
        QString masque = "Masque : ";
        masque += source.netmask().toString();
        ui->labelMasque->setText(masque);
    }
    else
    {
     /*   QMessageBox msgBox;
        msgBox.setText(ui->spinBoxOctet1->text()+"."+ui->spinBoxOctet2->text()+"."+ui->spinBoxOctet3->text()+"."+ui->spinBoxOctet4->text()+"Adresse IP Non Valide");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();*/
        ipValide=false;
    }

    //ip addr add 192.168.0.5/24 dev enp1s0
}

/**
 * @brief test si hote distant dispo
 * si oui envoyer requete
 * sinon smiley triste en local
 */
void Widget::on_pushButtonPing_clicked()
{

    QString destination=ui->spinBoxOctet1Contact->text()+"."+ui->spinBoxOctet2Contact->text()+
            "."+ui->spinBoxOctet3Contact->text()+"."+ui->spinBoxOctet4Contact->text();
    qDebug() << destination ;
    sock=new QTcpSocket(this);
    /* connect(sock,SIGNAL(connected()),this,SLOT(onQTcpSocketConnected()));
    connect(sock,SIGNAL(disconnected()),this,SLOT(onQTcpSocketDisconnected()));*/
    sock->connectToHost(destination,8888);
    if (sock->waitForConnected(1000))
    {
        onQTcpSocketConnected();
        sock->disconnectFromHost();
        if (sock->state() == QAbstractSocket::UnconnectedState ||
                sock->waitForDisconnected(1000))
        {
            qDebug()<<"deconnecte";
        }
    }
    else
    {
        allumerSmiley(false,Qt::red);
        onQTcpSocketDisconnected();
    }


}

void Widget::onQProcessFinished(int i, QProcess::ExitStatus etat)
{
    //ui->textEdit->append("adresse fixee");
    qDebug()<<"Etat : "<<etat;

    ipValide=true;
    ipFixee=true;
    majGroupBox(false,false,true);
    // lancement du serveur local
    sockServ=new QTcpServer();
    sockServ->listen(QHostAddress::Any,8888);
    connect(sockServ,SIGNAL(newConnection()),this,SLOT(onQTcpServerNewConnection()));


}

void Widget::onQTcpSocketConnected()
{
   /* QMessageBox msgBox;
    msgBox.setText("connecté");
    // msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();*/
    QBuffer tampon;
    tampon.open(QIODevice::WriteOnly);
    QDataStream out(&tampon);
    out<<couleurRaspi;
    sock->write(tampon.buffer());
}

void Widget::onQTcpSocketDisconnected()
{
   /* QMessageBox msgBox;
    msgBox.setText("deconnexion");
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();*/
    qDebug()<<"affichage smiley triste";
    allumerSmiley(false,couleurRaspi);
}

void Widget::Widget::onQProcessFinishedResetIp(int i, QProcess::ExitStatus etat)
{
    qDebug()<<"une adresse vire";
}

void Widget::onQTcpServerNewConnection()
{
    sockClientServ=sockServ->nextPendingConnection();

    connect(sockClientServ,SIGNAL(readyRead()),this,SLOT(onQTcpSocketReadyRead()));
    qDebug()<<"connexion de :  "<<sockClientServ->peerAddress().toString();
}

void Widget::onQTcpSocketReadyRead()
{
    QDataStream in(sockClientServ);
    QColor couleurDuContactant;
    in>>couleurDuContactant;
    //qDebug()<<demande;
    allumerSmiley(true,couleurDuContactant);

}
