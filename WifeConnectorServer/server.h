#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include <QTcpServer>
#include <QTcpSocket>

#include "tcpserver.h"

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
    void startServer();
    void stopServer();
    void broadcastMessage();
    void sendMessage(Connection *socket, const QString &message);

private:
    Ui::Server *ui;
    QNetworkSession *networkSession;
    TCPServer* tcpServer;
    static const quint16 port = 53703;

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
    void processReadMessage(Connection *socket);
};

#endif // SERVER_H
