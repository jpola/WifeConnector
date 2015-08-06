#include "connection.h"

Connection::Connection(QObject *parent) :
    QTcpSocket(parent), key(0)/*, socket(tcpSocket)*/
{
    key = rsHash(this->peerAddress(),
                 this->peerPort());

}

unsigned int Connection::getKey()
{
    return key;
}

//QTcpSocket* Connection::getSocket()
//{
//    return socket;
//}


//const QTcpSocket* Connection::getSocket() const
//{
//    return socket;
//}

unsigned int
Connection::rsHash(const QHostAddress &address, const quint16 port)
{
    unsigned int b    = 378551;
    unsigned int a    = 63689;
    unsigned int hash = 0;
    int i    = 0;

    QString key = address.toString() + QString(port);
    qDebug() << key;
    auto len = key.size();

    for (i = 0; i < len; i++)
    {
        hash = hash * a + key.at(i).toAscii();
        a = a * b;
    }

    return hash;
}
