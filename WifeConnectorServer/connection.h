#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
class Connection : public QTcpSocket
{
    Q_OBJECT

public:
    Connection(int descriptor, QObject *parent = 0);

    unsigned int getKey();

    int descriptor;
private:
    unsigned int key;
    static unsigned int counter;

    unsigned int rsHash(const QHostAddress &address, const quint16 port);

};

#endif // CONNECTION_H
