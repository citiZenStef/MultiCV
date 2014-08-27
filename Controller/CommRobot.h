#ifndef COMMROBOT_H
#define COMMROBOT_H

#include <QUdpSocket>
#include <iostream>

using namespace std;

class CommRobot
{
    static const QByteArray commandArray[];

public:
    CommRobot();
    ~CommRobot();

    bool check();
    bool tir();
    bool tirRafale();
    bool changePos(int x, int y);

private:
    void sendDatagram(int ID, int x = 0, int y = 0);
    void readDatagram();
    void editDatagram();

    enum {
        CHECK,
        TIR,
        RAFALE,
        BOUGE
    };

    QUdpSocket udpSocket;
    QHostAddress* host;
    QByteArray datagram;
};

#endif // COMMROBOT_H
