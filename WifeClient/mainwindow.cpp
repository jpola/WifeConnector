#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    ipAddress(QString("192.168.1.101"))

{
    ui->setupUi(this);

    ui->disconnectPushButton->setEnabled(false);

    QObject::connect(ui->connectPushButton, SIGNAL(clicked(bool)),
                     this, SLOT(connectToServer()));
    QObject::connect(ui->disconnectPushButton, SIGNAL(clicked(bool)),
                     this, SLOT(disconnectFromServer()));

    socket = new QTcpSocket(this);

    QObject::connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                     this, SLOT(socketStateChanged()));

    QObject::connect(socket, SIGNAL(readyRead()),
                     this, SLOT(processIncommingMessage()));

    QObject::connect(ui->inputLineEdit, SIGNAL(returnPressed()),
                     this, SLOT(processOutgoingMessage()));

}

void MainWindow::processIncommingMessage()
{
    if(socket == nullptr || socket->state() != QAbstractSocket::ConnectedState)
    {
        return;
    }

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_0);

    // This is flag which might be used somewhere else;
    // Zero it if you want to block the rest of server message
    quint16 blockSize(0);

    if (blockSize == 0)
    {

        // Check how many bytes are waiting to be read;
        // We are getting data in quint16
        if (socket->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> blockSize;
    }

    // If there are less than already available then this is not
    // what we should read;
    if (socket->bytesAvailable() < blockSize)
        return;

    QString message;
    in >> message;

    ui->displayTextEdit->append("Server says: " + message);
}

void MainWindow::updateGUI()
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        ui->connectPushButton->setEnabled(false);
        ui->disconnectPushButton->setEnabled(true);
        //ui->statusBar->showMessage("Connected");
        return;
    }

    if(socket->state() == QAbstractSocket::UnconnectedState)
    {
        ui->connectPushButton->setEnabled(true);
        ui->disconnectPushButton->setEnabled(false);
        //ui->statusBar->showMessage("Unconnected");
        return;
    }
}

void MainWindow::connectToServer()
{
    if(socket->state() != QAbstractSocket::ConnectedState)
    {
        socket->connectToHost(ipAddress, port);
    }

}

void MainWindow::disconnectFromServer()
{
    if(socket->state() != QAbstractSocket::UnconnectedState)
    {
        socket->disconnectFromHost();
    }

}

void MainWindow::processOutgoingMessage()
{
    QString outMessage = ui->inputLineEdit->text();

    sendMessage(outMessage);
    displayOutgoingMessage(outMessage);

}

void MainWindow::displayOutgoingMessage(const QString &message)
{
    ui->displayTextEdit->setTextColor(Qt::darkBlue);
    ui->displayTextEdit->append(message);
    ui->inputLineEdit->clear();
}

void MainWindow::sendMessage(const QString &message)
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    {

        QByteArray block;

        QDataStream out(&block, QIODevice::WriteOnly);

        out.setVersion(QDataStream::Qt_4_0);
        out << (quint16)0; //type of message
        out << message;
        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16)); //size of the sent message without the indicator

        socket->write(block);
    }
}

void MainWindow::socketStateChanged()
{
    using SState = QAbstractSocket;

    auto state = socket->state();
    QString strState;

    switch (state) {
    case SState::UnconnectedState:
        strState = "Unconnected.";
        break;
    case SState::HostLookupState:
        strState = "Host Lookup ...";
        break;
    case SState::ConnectingState:
        strState = "Connecting ...";
        break;
    case SState::ConnectedState:
        strState = "Connected.";
        break;
    case SState::BoundState:
        strState = "Bound.";
        break;
    case SState::ClosingState:
        strState = "Closing ...";
        break;
    case SState::ListeningState:
        strState = "Listening ...";
        break;
    default:
        strState = "Unknown ...";
        break;
    }

    ui->statusBar->showMessage(strState);
    updateGUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

