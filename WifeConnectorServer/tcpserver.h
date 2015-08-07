#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include "connection.h"
#include "connectionmanager.h"

class TCPServer : public QTcpServer
{
    QOBJECT_H
public:
    TCPServer(QObject *parent = 0);
    Connection* nextPendingConnectionC();
    ConnectionManager connectionManager;
private:

    void incomingConnection(int handle);

};

#endif // TCPSERVER_H
