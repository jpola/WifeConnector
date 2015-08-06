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

    QObject::connect(ui->inputLineEdit, SIGNAL(returnPressed()), this, SLOT(processMessage()));

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

void MainWindow::processMessage()
{
    QString msg = ui->inputLineEdit->text();

    sendMessage(msg);
    ui->displayTextEdit->append("MSG: " + msg);
    ui->inputLineEdit->clear();
}

void MainWindow::sendMessage(const QString &message)
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    {

        QByteArray block;

        QDataStream out(&block, QIODevice::WriteOnly);

        out.setVersion(QDataStream::Qt_4_0);
        out << (quint16)0;
        out << message;
        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));

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

