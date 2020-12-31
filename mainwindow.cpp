#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent,std::string ip_n,std::string user_n)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
{
        ui->setupUi(this);
        
         std::cout<<"Main window constructor was called"<<std::endl;
         ip_addres = ip_n;
         user_name = user_n;         
         this->mw_udp_socket = new Udp_Socket(ip_addres);
         this->mw_tcp_socket = new Tcp_Socket(ip_addres);
         //this->mw_udp_socket->socket_create();
         //this->mw_udp_socket->start_listen();
         this->mw_tcp_socket->create_socket();
         //this->mw_tcp_socket->socket_listen();
         tcp_listen_th = new std::thread([this](){mw_tcp_socket->socket_listen();});
}

MainWindow::~MainWindow()
{
        delete ui;
}
