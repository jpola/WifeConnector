#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include <QTcpServer>
#include <QTcpSocket>
#include "connection.h"
namespace Ui {
class Server;
}

class Server : public QMainWindow
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);
    ~Server();

public slots:
    void saveSessionConfiguration();
    bool createTcpServer();
    void readMessage();
    void handleConnection();
    void removeSocket(QTcpSocket *socket);
    void startServer();
    void stopServer();
    void broadcastMessages(const QString &message);
    void sendMessage(QTcpSocket *socket, const QString &message);

private:
    Ui::Server *ui;
    QNetworkSession *networkSession;
    QTcpServer* tcpServer;
    static const quint16 port = 53703;

    QMap<unsigned int, Connection*> tcpSockets;
    QTcpSocket* tcpSocket;


    QNetworkConfigurationManager networkManager;
    QNetworkConfiguration config;

    void setLogInfo(const QString &message);

    void displayNetworkConfigurations
                           (const QNetworkConfigurationManager& networkManager);

    QNetworkConfiguration getNetworkConfiguration();

    bool createNetworkSession(const QNetworkConfiguration& config);

private slots:
    void networkSessionStateChanged();
    void socketStateChanged(QAbstractSocket::SocketState state);
    void processReadMessage(QTcpSocket *socket);
};

#endif // SERVER_H
