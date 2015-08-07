#include "connectionmanager.h"

ConnectionManager::ConnectionManager(QObject *parent) :
    QObject(parent)
{

}
ConnectionManager::~ConnectionManager()
{
    qDebug() << "Pending connections in connection manager " << connections.size();
    for(auto descriptor : connections.keys())
    {
        Connection *connection = connections.value(descriptor);
        if(connection)
        {
            qDebug() << "Removing: " << descriptor;
            connections.remove(descriptor);
            connection->deleteLater();
        }
    }
}

QList<unsigned int> ConnectionManager::descriptors()
{
    return connections.keys();
}

int ConnectionManager::nConnections()
{
    return connections.size();
}

void ConnectionManager::addConnection(Connection *connection)
{
    QObject::connect(connection, SIGNAL(disconnected()), this, SLOT(removeConnection()));
    connections.insert(connection->socketDescriptor(), connection);
    qDebug() << "Number of connections " << connections.size();
}


void ConnectionManager::removeConnection(int descriptor)
{
    Connection* connection = connections.value(descriptor);
    if(connection)
    {
        connections.remove(descriptor);
        connection->deleteLater();
    }
}

void ConnectionManager::removeConnection()
{
    Connection *connection = qobject_cast<Connection*>(sender());
    if(connection)
    {
        qDebug() << "Removing socket descriptor: " << connection->descriptor;
        connections.remove(connection->descriptor);
        connection->deleteLater();
        qDebug() << "Number of connections " << connections.size();
    }


}

Connection* ConnectionManager::getConnection(int descriptor)
{

    Connection* connection = connections.value(descriptor);
    if(connection)
        return connection;
    else return nullptr;

}
