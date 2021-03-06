
#include "server.h"
#include "ui_server.h"
#include <QSettings>

#include "networkproperiesutils.hpp"

Server::Server(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Server),
    networkManager(this)
{
    ui->setupUi(this);

    QObject::connect(ui->startServerPushButton, SIGNAL(clicked(bool)), this, SLOT(startServer()));
    QObject::connect(ui->stopServerPushButton,  SIGNAL(clicked(bool)), this, SLOT(stopServer()));
    QObject::connect(ui->broadcastLineEdit, SIGNAL(returnPressed()), this, SLOT(broadcastMessage()));

    networkSession = nullptr;
    tcpServer = nullptr;
}

void Server::startServer()
{
    if( tcpServer == nullptr)
    {
        tcpServer = new TCPServer(this);
    }

    if (!tcpServer->isListening())
    {
        bool isOnline = networkManager.isOnline();
        setLogInfo(tr("System connection availability: %1").
                   arg(isOnline ? tr("OK") : tr("KO"))) ;

        if (!isOnline)
        {
            setLogInfo(tr("Fix internet connection and try again"));
            return;
        }

        displayNetworkConfigurations(networkManager);
        config = getNetworkConfiguration();

        // create network session and save configuration by connecting
        // to slot sessionOpened();
        if (!createNetworkSession(config))
        {
            return;
        }

        // create tcp server;
        if(!createTcpServer())
        {
            return;
        }

        QObject::connect(tcpServer, SIGNAL(newConnection()), this, SLOT(handleConnection()));
    }
}

void Server::stopServer()
{
    if (tcpServer == nullptr)
    {
        setLogInfo(tr("TCP Server is not created yet. Start the server first"));
        return;
    }

    if (tcpServer->isListening())
    {
        setLogInfo("Server shutting down");
        tcpServer->close();
        delete tcpServer;
        tcpServer = nullptr;
    }
}


void Server::handleConnection()
{

    auto *socket = tcpServer->nextPendingConnectionC();
    if (!socket)
    {
        setLogInfo(tr("Unable create connection"));
        return;
    }

    setLogInfo(tr("New connection: %1").arg(socket->getKey()));

    connect(socket, SIGNAL(readyRead()), this, SLOT(readMessage()));

    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this,
            SLOT(socketStateChanged(QAbstractSocket::SocketState)));

}

void Server::socketStateChanged(QAbstractSocket::SocketState state)
{
    Connection *connection = qobject_cast<Connection *>(sender());

    if (connection)
    {
        if (state == QAbstractSocket::ClosingState)
        {
            setLogInfo(tr("Socket with descriptor %1 is about to be closed").
                       arg(connection->descriptor));
            processReadMessage(connection);
        }
    }
}


void Server::broadcastMessage()
{
    QString message = ui->broadcastLineEdit->text();

    for( auto descriptor: tcpServer->connectionManager.descriptors() )
    {
        Connection *connection =
                tcpServer->connectionManager.getConnection(descriptor);
        if(connection)
        {
            qDebug() << "Sending " << message << " to " << descriptor;
            sendMessage(connection, message);
        }
    }

    ui->broadcastLineEdit->clear();
}

void Server::sendMessage(Connection *connection, const QString &message)
{
    if(connection->state() == QAbstractSocket::ConnectedState)
    {
        QByteArray block;

        QDataStream out(&block, QIODevice::WriteOnly);

        out.setVersion(QDataStream::Qt_4_0);
        out << (quint16)0;
        out << message;
        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));

        connection->write(block);
    }

}

void Server::readMessage()
{
    Connection *connection = qobject_cast<Connection *>(sender());
    processReadMessage(connection);
}

void Server::processReadMessage(Connection *socket)
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_0);

    quint16 blockSize = 0;

    if (blockSize == 0)
    {
        if (socket->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> blockSize;
    }

    if (socket->bytesAvailable() < blockSize)
        return;

    QString msg;
    in >> msg;

    auto senderIpAddress = socket->peerAddress();

    setLogInfo(tr("%1 : %2").arg(senderIpAddress.toString(), msg));

}

Server::~Server()
{
    delete ui;
    if (tcpServer && tcpServer->isListening())
    {
        tcpServer->close();
    }
    delete tcpServer;

    if( networkSession->isOpen())
    {
        networkSession->close();
    }

    delete networkSession;

}

void Server::setLogInfo(const QString &message)
{
    ui->logTextEdit->append(message);
}

void Server::displayNetworkConfigurations(const QNetworkConfigurationManager& networkManager)
{
    //Let's see what configs our system have
    QList<QNetworkConfiguration> liConfig = networkManager.allConfigurations();

    setLogInfo(tr("Network config count: %1").arg(liConfig.size()));

    int i = 0;
    for (auto configs : liConfig)
    {
        setLogInfo(tr("Networ config #: %1").arg(i++));
        setLogInfo(tr("\tID: %1").arg( configs.identifier() ) ) ;
        setLogInfo(tr("\tBearer name: %1").arg(configs.bearerTypeName()));
        setLogInfo(tr("\tBearer type: %1").arg(configs.bearerType()));
        setLogInfo(tr("\tValidity: %1").arg(configs.isValid()));
        setLogInfo(tr("\tName: %1").arg(configs.name()));
        setLogInfo(tr("\tState: %1").arg(getStrNetConfigState(configs.state())));
        setLogInfo(tr("\tType: %1").arg(getStrNetConfigType(configs.type())));
        setLogInfo("\n");
    }
}

QNetworkConfiguration Server::getNetworkConfiguration()
{
    QSettings settings(QSettings::UserScope, "WifeServer");
    settings.beginGroup("NetworkConfiguration");
    const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration"))
            .toString();
    settings.endGroup();

    QNetworkConfiguration configuration;
    if (!id.isEmpty())
    {
        setLogInfo(tr("Setting configuration from settings id: %1").arg(id));
        configuration = networkManager.configurationFromIdentifier(id);
    }
    else
    {
        setLogInfo(tr("Setting configuration from default."));
        configuration = networkManager.defaultConfiguration();
    }

    return configuration;
}

bool Server::createNetworkSession(const QNetworkConfiguration &config)
{
    setLogInfo(tr("Creating new session"));

    if (networkSession != nullptr)
    {
        networkSession->close();
        delete networkSession;
    }

    networkSession = new QNetworkSession(config, this);

    QObject::connect(networkSession, SIGNAL(opened()), this,
                     SLOT(saveSessionConfiguration()));
    QObject::connect(networkSession, SIGNAL(stateChanged(QNetworkSession::State)),
                     this, SLOT(networkSessionStateChanged()));

    networkSession->open();

    return true;
}

void Server::networkSessionStateChanged()
{
    auto state = networkSession->state();
    QString strState;

    switch (state) {
    case QNetworkSession::Invalid:
        strState = "Invalid";
        break;
    case QNetworkSession::NotAvailable:
        strState = "NotAvailable";
        break;
    case QNetworkSession::Connecting:
        strState = "Connecting";
        break;
    case QNetworkSession::Connected:
        strState = "Connected";
        break;
    case QNetworkSession::Closing:
        strState = "Closing";
        break;
    case QNetworkSession::Disconnected:
        strState = "Disconnected";
        break;
    case QNetworkSession::Roaming:
        strState = "Roaming";
        break;
    default:
        strState = "Unknown";
        break;
    }

    setLogInfo(tr("Network session changed to %1 state").arg(strState));

}

void Server::saveSessionConfiguration()
{
    //save configuration
    if (networkSession)
    {
        auto config = networkSession->configuration();
        QString id;
        if (config.type() == QNetworkConfiguration::UserChoice)
            id = networkSession->
                    sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
        else
            id = config.identifier();

        QSettings settings(QSettings::UserScope, QLatin1String("WifeServer"));
        settings.beginGroup(QLatin1String("NetworkConfiguration"));
        settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
        settings.endGroup();

        setLogInfo(tr("Session configuration saved"));
        return;
    }

    setLogInfo(tr("Session not yet available. Configuration not saved"));
}

bool Server::createTcpServer()
{
    QString strIpAddress;
    QHostAddress ipAddress;


    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i);
            setLogInfo(tr("Non local IPv4 address found"));
            break;
        }
    }

    if(ipAddress == QHostAddress::Null)
    {
        setLogInfo(tr("Non local IPv4 address not found. Using localhost"));
        ipAddress = QHostAddress::LocalHost;
    }

    strIpAddress = ipAddress.toString();

    tcpServer = new TCPServer(this);
    if (!tcpServer->listen(ipAddress, port))
    {
        setLogInfo(tr("Unable to start TCP server: %1")
                   .arg(tcpServer->errorString()));
        return false;
    }

    setLogInfo(tr("The server is running on\n\tIP: %1\n\tport: %2\n\n")
               .arg(strIpAddress).arg(tcpServer->serverPort()));
    return true;
}
