#include "connectionstartdialog.h"
#include "ui_connectionstartdialog.h"

ConnectionStartDialog::ConnectionStartDialog(QWidget *parent, Tcp_Socket *tcp, std::string username, std::string client_ip) :
    QDialog(parent),
    ui(new Ui::ConnectionStartDialog)
{
    ui->label->setText(QString::fromStdString(username) + "wants to talk to you. Do you accept?");
    this->client_ip = client_ip;
    this->tcp = tcp;
    ui->setupUi(this);
}

ConnectionStartDialog::~ConnectionStartDialog()
{
    delete ui;
}

void ConnectionStartDialog::on_pushButton_2_clicked()
{
    //yes
    this->tcp->send_message(this->tcp->ip_assigned,client_ip,CONNECTION_ACCEPT);
    this->close();
}

void ConnectionStartDialog::on_pushButton_clicked()
{
    //no
    this->tcp->send_message(this->tcp->ip_assigned,client_ip,CONNECTION_REFUSED);
    this->close();
}
