#include "connectionstartdialog.h"
#include "ui_connectionstartdialog.h"

ConnectionStartDialog::ConnectionStartDialog(QWidget *parent, Tcp_Socket *tcp, QString username, QString client_ip, Active_Clients *act_client) :
    QDialog(parent),
    ui(new Ui::ConnectionStartDialog)
{
    ui->setupUi(this);
    ui->label->setText(username + " wants to talk to you. Do you accept?");
    this->client_ip =  client_ip.toStdString();
    this->username = username.toStdString();
    this->tcp = tcp;
    this->act_client = act_client;
}

ConnectionStartDialog::~ConnectionStartDialog()
{
    delete ui;
}

void ConnectionStartDialog::on_pushButton_2_clicked()
{
    //yes
    act_client->active_client_ip_addr = client_ip;
    act_client->active_client_host_name = username;
    this->tcp->is_connected = true;
    this->tcp->send_message(this->tcp->user_name,client_ip,CONNECTION_ACCEPT);
    this->close();
}

void ConnectionStartDialog::on_pushButton_clicked()
{
    //no
    this->tcp->send_message(this->tcp->user_name,client_ip,CONNECTION_REFUSED);
    this->close();
}
