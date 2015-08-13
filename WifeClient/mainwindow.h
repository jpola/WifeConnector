#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTcpSocket* socket;

    QHostAddress ipAddress;
    static const quint16 port = 53703;

private slots:
    void socketStateChanged();
    void processOutgoingMessage();
    void processIncommingMessage();
    void sendMessage(const QString &message);
    void connectToServer();
    void disconnectFromServer();
    void updateGUI();
    void displayOutgoingMessage(const QString &message);

};

#endif // MAINWINDOW_H
