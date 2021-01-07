#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <ctime>
#include <sstream>
#include <thread>

MainWindow::MainWindow(QWidget *parent,std::string ip_n,std::string user_n)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
{
        ui->setupUi(this);

         std::cout<<"Main window constructor was called"<<std::endl;
         ip_addres = ip_n;
         user_name = user_n;
         this->mw_udp_socket = new Udp_Socket(ip_addres, &mw_act_clients);
         this->mw_tcp_socket = new Tcp_Socket(user_name, ip_addres, &mw_act_clients);
         
         connect(this->mw_tcp_socket,&Tcp_Socket::new_msg_came,this,&MainWindow::msg_message_rcv);
         connect(this->mw_tcp_socket,&Tcp_Socket::new_msg_online,this,&MainWindow::msg_message_onl);
         connect(this->mw_tcp_socket,&Tcp_Socket::new_msg_accepted_clr,this,&MainWindow::msg_box_clr);
         connect(this->mw_tcp_socket,&Tcp_Socket::connectionStart,this,&MainWindow::connectionStart);
         connect(this->mw_tcp_socket,&Tcp_Socket::connectionAccepted,this,&MainWindow::connectionAccepted);
         connect(this->mw_tcp_socket,&Tcp_Socket::connectionRefused,this,&MainWindow::connectionRefused);
         connect(this->mw_tcp_socket,&Tcp_Socket::connectionStopped,this,&MainWindow::connectionStopped);
}

MainWindow::~MainWindow()
{
        delete ui;
}

std::string add_zero(int i) {
    std::stringstream ss;
    if (i >= 0 && i <= 9) {
        ss << "0" << i;
    } else {
        ss << i;
    }
    return ss.str();
}

// Her discover ve respond mesajı geldiğinde aktif clienta ekle. Thread
// Her mesaj geldiğinde yazdır. Thread


void MainWindow::on_sendMsgButton_clicked()
{
    QString message = ui->msgTextEdit->toPlainText();
    if (message.isEmpty() == false) {
        ui->msgTextEdit->clear();

        time_t now = time(0);
        tm *ltm = localtime(&now);
        std::string ttime = "[" + add_zero(ltm->tm_hour) + ":" + add_zero(ltm->tm_min) + ":" + add_zero(ltm->tm_sec) + "]";

        if(this->mw_tcp_socket->is_connected == true) {
            ui->msgListWidget->addItem(QString::fromStdString(ttime) + " " + QString::fromStdString(user_name) + ": " + message);
            this->mw_tcp_socket->send_message(message.toStdString(),this->mw_act_clients.active_client_ip_addr,CONNECTION_MESSAGE);
        }
        
    }
}



void MainWindow::on_activeClientsList_itemDoubleClicked(QListWidgetItem *item)
{
    if(this->mw_tcp_socket->is_connected == true){
        this->mw_tcp_socket->send_message("", this->mw_act_clients.active_client_ip_addr,CONNECTION_STOP);
    }
    this->mw_tcp_socket->is_connected = false;
    this->mw_act_clients.active_client_ip_addr      = "";
    this->mw_act_clients.active_client_host_name    = "";

    for(int i = 0 ;i<this->mw_act_clients.online_clients.size();i++){
        if(this->mw_act_clients.online_clients.at(i).first == item->text().toStdString())
        {
            this->mw_tcp_socket->send_connection_request(this->mw_act_clients.online_clients.at(i).second);
            break;
        }
    }
}

void MainWindow::msg_message_onl(QString username) {

    QList<QListWidgetItem *> items = ui->activeClientsList->findItems(username, Qt::MatchExactly);
    if(items.size() == 0) {
        ui->activeClientsList->addItem(username);
    }
    
}

void MainWindow::msg_message_rcv(QString msg){
    time_t now = time(0);
    tm *ltm = localtime(&now);
    std::string ttime = "[" + add_zero(ltm->tm_hour) + ":" + add_zero(ltm->tm_min) + ":" + add_zero(ltm->tm_sec) + "]";
    ui->msgListWidget->addItem(QString::fromStdString(ttime) + " " + QString::fromStdString(this->mw_act_clients.active_client_host_name) + ": " + msg);
    
}


void MainWindow::on_activateClientButton_clicked()
{
    
    QListWidgetItem *temp_item = this->ui->activeClientsList->currentItem();
    
    if(temp_item !=NULL){
        
        
        if(this->mw_tcp_socket->is_connected == true){
            this->mw_tcp_socket->send_message("", this->mw_act_clients.active_client_ip_addr,CONNECTION_STOP);
        }
        this->mw_tcp_socket->is_connected = false;
        this->mw_act_clients.active_client_ip_addr      = "";
        this->mw_act_clients.active_client_host_name    = "";
        
        for(int i = 0 ;i<this->mw_act_clients.online_clients.size();i++){
            if(this->mw_act_clients.online_clients.at(i).first == temp_item->text().toStdString())
            {
                this->mw_tcp_socket->send_connection_request(this->mw_act_clients.online_clients.at(i).second);
                break;
            }
        }
    }
    
}

void MainWindow::msg_box_clr()
{
    ui->msgListWidget->clear();
}

void MainWindow::on_stopClientButton_clicked()
{
    if(this->mw_tcp_socket->is_connected == true ){
        this->mw_tcp_socket->is_connected = false;
        this->mw_tcp_socket->send_message(this->mw_tcp_socket->user_name,this->mw_act_clients.active_client_ip_addr,CONNECTION_STOP);
        this->mw_act_clients.active_client_host_name = "";
        this->mw_act_clients.active_client_ip_addr   = "";
        ui->msgListWidget->clear();
    }
}

void MainWindow::connectionStart(QString username, QString client_ip) {
    ConnectionStartDialog dialog(this, this->mw_tcp_socket, username, client_ip);
    dialog.setModal(true);
    dialog.exec();
}
void MainWindow::connectionAccepted(QString username) {
    ConnectionAcceptedDialog dialog(this, username);
    dialog.setModal(true);
    dialog.exec();
}
void MainWindow::connectionRefused(QString username) {
    ConnectionRefusedDialog dialog(this, username);
    dialog.setModal(true);
    dialog.exec();
}
void MainWindow::connectionStopped(QString username) {
    ConnectionStopDialog dialog(this, username);
    dialog.setModal(true);
    dialog.exec();
}
