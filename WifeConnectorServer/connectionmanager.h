#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include "connection.h"

class ConnectionManager : public QObject
{
    Q_OBJECT
public:
    ConnectionManager(QObject *parent = 0);
    ~ConnectionManager();
    void addConnection(Connection *connection);
    void removeConnection(int descriptor);
    int  nConnections();
    Connection* getConnection(int descriptor);
    QList<unsigned int> descriptors();


public slots:

    void removeConnection();

private:
    QMap<unsigned int, Connection*> connections;
};

#endif // CONNECTIONMANAGER_H
