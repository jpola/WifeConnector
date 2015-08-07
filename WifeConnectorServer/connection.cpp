#include "connection.h"

unsigned int Connection::counter = 0;

Connection::Connection(int descriptor, QObject *parent) :
    QTcpSocket(parent), key(0), descriptor(descriptor)/*, socket(tcpSocket)*/
{
    /* First we havet to set the descriptor for socket.
     * It is very important because we are reaching to socketEngine
     * where the address many parameters are set ie. peer port and address;
     *
     * see here: http://download.froglogic.com/public/qt5-squishcoco-report/QtBase/source_480_preprocessed.html
     */

    qDebug() << "Creating descriptor " << descriptor;
    this->setSocketDescriptor(descriptor);

    counter += 1;
    key = rsHash(this->peerAddress(),
                 this->peerPort());

}

unsigned int Connection::getKey()
{
    return key;
}

unsigned int
Connection::rsHash(const QHostAddress &address, const quint16 port)
{
    unsigned int b    = 378551;
    unsigned int a    = 63689;
    unsigned int hash = 0;
    int i    = 0;
    QString key(address.toString() + QString(port) + QString(counter));

    auto len = key.size();

    for (i = 0; i < len; i++)
    {
        hash = hash * a + key.at(i).toAscii();
        a = a * b;
    }

    return hash;
}
