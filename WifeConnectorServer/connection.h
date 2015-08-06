#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
class Connection : QTcpSocket
{
    Q_OBJECT

public:
    Connection(QObject *parent = 0);

    unsigned int getKey();
//    QTcpSocket* getSocket();
//    const QTcpSocket* getSocket() const;

private:

    //unique id of the socket;
    unsigned int key;
//    QTcpSocket* socket;
    unsigned int rsHash(const QHostAddress &address, const quint16 port);

};

#endif // CONNECTION_H
