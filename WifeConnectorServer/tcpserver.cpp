#include "tcpserver.h"

TCPServer::TCPServer(QObject *parent) :
    QTcpServer(parent), connectionManager(this)
{

}


void TCPServer::incomingConnection(int handle)
{
    Connection *connection = new Connection(handle, this);

    connectionManager.addConnection(connection);


    this->addPendingConnection(connection);
}

Connection* TCPServer::nextPendingConnectionC()
{

    auto *s = this->nextPendingConnection();
    if(!s)
        return nullptr;

    return connectionManager.getConnection(s->socketDescriptor());
}


