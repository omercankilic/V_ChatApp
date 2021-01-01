#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent,std::string ip_n,std::string user_n)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
{
        ui->setupUi(this);
        
         ip_addres = ip_n;
         user_name = user_n;        
         mw_act_clients= new Active_Clients();
         this->mw_udp_socket = new Udp_Socket(ip_addres,mw_act_clients);
         this->mw_tcp_socket = new Tcp_Socket(ip_addres,mw_act_clients);
        
}

MainWindow::~MainWindow()
{
        delete ui;
}
