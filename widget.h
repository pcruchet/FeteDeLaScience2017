#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QNetworkInterface>
#include <QTimer>
#include <QProcess>
#include <QTcpSocket>
#include <QTcpServer>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    Widget(QString coul,QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pushButtonEtat_clicked();

    void on_pushButtonAdresse_clicked();
    void onQTimerIsRunning();

    void on_pushButtonSetIp_clicked();

    void on_pushButtonPing_clicked();
    void onQProcessFinished(int i, QProcess::ExitStatus etat);
    void onQTcpSocketConnected();
    void onQTcpSocketDisconnected();
    void onQProcessFinishedResetIp(int i, QProcess::ExitStatus etat);

    void onQTcpServerNewConnection();
    void onQTcpSocketReadyRead();


private:

    Ui::Widget *ui;
    QNetworkInterface interfaceActive;
    //int index;
    QTimer *timerEtatInterface;
    QTcpSocket *sock;
    QNetworkAddressEntry ipOrigine;
    bool ipValide;
    QColor couleurRaspi;
    QTcpServer *sockServ;
    QTcpSocket *sockClientServ;


    void eteindreLeds();
    void allumerSmiley(bool happy, QColor couleur);
    void majGroupBox(bool cable, bool ip, bool ping);
    bool verifIp();
    void reInitIp();
    void allumerEtat(QColor couleurLed);
    bool ipFixee;

};

#endif // WIDGET_H
