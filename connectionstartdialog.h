#ifndef CONNECTIONSTARTDIALOG_H
#define CONNECTIONSTARTDIALOG_H

#include <QDialog>
#include <Tcp_Socket/TCP_SOCKET.h>

using namespace Chat;

namespace Ui {
class ConnectionStartDialog;
}

class ConnectionStartDialog : public QDialog
{
    Q_OBJECT

public:
    ConnectionStartDialog(QWidget *parent = nullptr, Tcp_Socket *tcp = nullptr, QString username ="", QString client_ip = "");
    ~ConnectionStartDialog();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::ConnectionStartDialog *ui;
    Tcp_Socket *tcp;
    std::string client_ip;
    std::string username;
};

#endif // CONNECTIONSTARTDIALOG_H
