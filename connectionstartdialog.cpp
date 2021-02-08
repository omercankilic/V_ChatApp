#include "connectionstartdialog.h"
#include "ui_connectionstartdialog.h"

ConnectionStartDialog::ConnectionStartDialog(QWidget *parent, Tcp_Socket *tcp, QString username, QString client_ip, Active_Clients *act_client) :
    QDialog(parent),
    ui(new Ui::ConnectionStartDialog)
{
    ui->setupUi(this);
    ui->label->setText(username + " wants to talk to you. Do you accept?");
    is_video_dialog =false;
    this->client_ip =  client_ip.toStdString();
    this->username = username.toStdString();
    this->tcp = tcp;
    this->act_client = act_client;
}

ConnectionStartDialog::ConnectionStartDialog(QWidget *parent, Tcp_Socket *tcp,bool is_video):
    QDialog(parent),
    ui(new Ui::ConnectionStartDialog)
{   
    ui->setupUi(this);
    ui->label->setText(QString::fromStdString(tcp->act_clients->active_client_host_name) + " wants to make video call. Do you accept?");
    is_video_dialog = true;
    this->tcp = tcp;
    this->client_ip = this->tcp->act_clients->active_client_ip_addr;
    this->username  = this->tcp->act_clients->active_client_host_name;
}

ConnectionStartDialog::~ConnectionStartDialog()
{
    delete ui;
}
//SAY YES
//TO accept video call and other connection requests 
void ConnectionStartDialog::on_pushButton_2_clicked()
{
    if(is_video_dialog == true){
       
        is_video_dialog = false;
        this->tcp->is_connected = true;
        this->tcp->send_message(this->tcp->user_name,this->client_ip,CONNECTION_VIDEO_ACCEPTED);
        emit this->tcp->new_msg_video_call_response(true);
        this->close();
    }else{
        act_client->active_client_ip_addr = client_ip;
        act_client->active_client_host_name = username;
        this->tcp->is_connected = true;
        this->tcp->send_message(this->tcp->user_name,client_ip,CONNECTION_ACCEPT);
        this->close();
    }
}
//SAY NO
//TO refuse video call and other connection requests 
void ConnectionStartDialog::on_pushButton_clicked()
{
    //no
    if(is_video_dialog == true){
        is_video_dialog = false;
        this->tcp->is_connected = true;
        this->tcp->send_message(this->tcp->user_name,this->client_ip,CONNECTION_VIDEO_REFUSED);
        this->close();
    }else{
        this->tcp->send_message(this->tcp->user_name,client_ip,CONNECTION_REFUSED);
        this->close();
    }
}
