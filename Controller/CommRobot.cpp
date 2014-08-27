#include "CommRobot.h"

const QByteArray CommRobot::commandArray[] = {QByteArray("AAA",3),
                                              QByteArray("BBB",3),
                                              QByteArray("CCC",3),
                                              QByteArray("DDD",3)};


CommRobot::CommRobot()
{
    host = new QHostAddress("192.168.0.2");
    udpSocket.bind(6625);
    datagram.resize(3);
}


bool CommRobot::check()
{
    this->sendDatagram(CHECK);
    // lire reponse robot : operationnel : 0x61 0x61 0x61
    udpSocket.readDatagram(datagram.data(),datagram.size());
    if(!strcmp(datagram.data(),"AAA")) {
        cout << "init OK" << endl;
        return true;
    }
    else {
        cout << "Pas de reponse a l initialisation" << endl;
        return false;
    }
}


bool CommRobot::tir()
{
    this->sendDatagram(TIR);
    // lire reponse robot : tir OK 0x62 0x62 0x62
    udpSocket.readDatagram(datagram.data(),datagram.size());
    if(!strcmp(datagram.data(),"BBB")) {
        cout << "tir OK" << endl;
        return true;
    }
    else {
        cout << "Pas de reponse au tir" << endl;
        return false;
    }
}


bool CommRobot::tirRafale()
{
    this->sendDatagram(RAFALE);
    // lire reponse robot : tir rafale OK 0x63 0x63 0x63
    udpSocket.readDatagram(datagram.data(),datagram.size());
    if(!strcmp(datagram.data(),"CCC")) {
        cout << "tir rafale OK" << endl;
        return true;
    }
    else {
        cout << "Pas de reponse au tir en rafale" << endl;
        return false;
    }
}


bool CommRobot::changePos(int x, int y)
{
    this->sendDatagram(BOUGE, x, y);
    // lire reponse robot : mvt OK : 0x64 0x64 0x64
    udpSocket.readDatagram(datagram.data(),datagram.size());
    if(!strcmp(datagram.data(),"DDD")) {
        cout << "changement position OK" << endl;
        return true;
    }
    else {
        cout << "Pas de reponse au changement de position" << endl;
        return false;
    }
}


void CommRobot::sendDatagram(int ID, int x, int y)
{
    if(x == 0 && y == 0) {
        this->datagram = commandArray[ID];
    }
    else {
        this->datagram = commandArray[ID];
        this->datagram[1] = x;
        this->datagram[2] = y;
    }

    udpSocket.writeDatagram(datagram,3,*host,6625);
}
