#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <ctime>
#include <sstream>
#include <thread>

MainWindow::MainWindow(QWidget *parent,std::string ip_n,std::string user_n)
    : QMainWindow(parent), 
      ui(new Ui::MainWindow)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    std::cout<<"Main window constructor was called"<<std::endl;
    ip_addres = ip_n;
    user_name = user_n;
    this->mw_tcp_socket = new Tcp_Socket(user_name, ip_addres, &mw_act_clients);
    this->mw_udp_socket = new Udp_Socket(ip_addres, &mw_act_clients,this->user_name);
    
    connect(this->mw_udp_socket,&Udp_Socket::new_msg_online,this,&MainWindow::msg_message_onl);
    connect(this->mw_udp_socket,&Udp_Socket::udp_respond_signal,this,&MainWindow::sendRespondMsg);
    this->mw_vc_create();
    //signal slot connections
    connect(this->mw_tcp_socket,&Tcp_Socket::new_msg_came,this,&MainWindow::msg_message_rcv);
    connect(this->mw_tcp_socket,&Tcp_Socket::new_msg_online_tcp,this,&MainWindow::msg_message_onl);
    connect(this->mw_tcp_socket,&Tcp_Socket::new_msg_accepted_clr,this,&MainWindow::msg_box_clr);
    connect(this->mw_tcp_socket,&Tcp_Socket::connectionStart,this,&MainWindow::connectionStart);
    //TCP to videoCall events
    connect(this->mw_tcp_socket,&Tcp_Socket::new_msg_video_call_response,this->mw_video_call,&VideoCall::video_call_accept_reject);
    connect(this->mw_tcp_socket,&Tcp_Socket::connectionNotification,this,&MainWindow::connectionNotification);
    
    connect(this->mw_video_call,&VideoCall::video_call_request_signal,this->mw_tcp_socket,&Tcp_Socket::send_video_message);
    
    //connect(this->mw_tcp_socket,&Tcp_Socket::new_msg_deactive,this->mw_video_call,&VideoCall::deactivated);
    //connect(this->mw_tcp_socket,&Tcp_Socket::new_msg_activate,this->mw_video_call,&VideoCall::activated);
    //connect(this->mw_video_call,&VideoCall::vc_start,this->mw_tcp_socket,&Tcp_Socket::send_video_connection_request);
    ui->setupUi(this);
    
}

MainWindow::~MainWindow()
{
    if(this->mw_tcp_socket->is_connected == true){
        
        this->mw_tcp_socket->send_message("",this->mw_act_clients.active_client_ip_addr,CONNECTION_STOP);
        if(this->mw_video_call !=nullptr){
            if(this->mw_video_call->video_call_connected){
                (this->mw_video_call->~VideoCall());
            }else{
                this->mw_video_call->close();
            }
        }
    }else{
        if(this->mw_video_call != nullptr){
            this->mw_video_call->~VideoCall();
        }
    }
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
        //TODO: send signal to video call to stop stop video sending
        //then break connection with 
        
        this->mw_tcp_socket->is_connected = false;
        this->mw_tcp_socket->send_message(this->mw_tcp_socket->user_name,this->mw_act_clients.active_client_ip_addr,CONNECTION_STOP);
        this->mw_act_clients.active_client_host_name = "";
        this->mw_act_clients.active_client_ip_addr   = "";
        ui->msgListWidget->clear();
    }
}

void MainWindow::sendVideoStopMsg()
{
    
}

void MainWindow::sendVideoPausedMsg()
{
    
}

void MainWindow::sendVideoContinueMsg()
{
    
}

void MainWindow::sendVideoStartMsg()
{
    
}

void MainWindow::sendRespondMsg(QString target_ip)
{
    this->mw_tcp_socket->send_message(user_name,target_ip.toStdString(),CONNECTION_RESPOND);
}

void MainWindow::connectionStart(QString username, QString client_ip) {
    ConnectionStartDialog *dialog = new ConnectionStartDialog(this, this->mw_tcp_socket, username, client_ip, &mw_act_clients);
    dialog->show();
}

void MainWindow::connectionNotification(QString notification) {
    connectionNotificationDialog *dialog = new connectionNotificationDialog(this, notification);
    dialog->show();
}
void MainWindow::mw_vc_create(){
    QWidget *vid_widget = nullptr;
    mw_video_call = new VideoCall(vid_widget,mw_act_clients.active_client_ip_addr,23000,this->mw_tcp_socket);
    
}
void MainWindow::on_videoButton_clicked()
{
    mw_video_call->show();
}
