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
        ui->activeClientsList->addItem("omercan");

         std::cout<<"Main window constructor was called"<<std::endl;
         ip_addres = ip_n;
         user_name = user_n;
         this->mw_udp_socket = new Udp_Socket(ip_addres, &mw_act_clients);
         this->mw_tcp_socket = new Tcp_Socket(ip_addres, &mw_act_clients);
         message_receive = new std::thread( [this](){ message_received();});
         active_client = new std::thread( [this](){ active_clients();});
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
        ui->msgListWidget->addItem(QString::fromStdString(ttime) + " " + QString::fromStdString(user_name) + ": " + message);

        if(this->mw_tcp_socket->is_connected == true) {
            this->mw_tcp_socket->send_message(message.toStdString());
        }
    }
}



void MainWindow::on_activeClientsList_itemDoubleClicked(QListWidgetItem *item)
{
    ui->msgListWidget->clear();
}

void MainWindow::message_received() {
    while(true) {
        if (this->mw_tcp_socket->message.size() != 0) {
            time_t now = time(0);
            tm *ltm = localtime(&now);
            std::string ttime = "[" + add_zero(ltm->tm_hour) + ":" + add_zero(ltm->tm_min) + ":" + add_zero(ltm->tm_sec) + "]";
            ui->msgListWidget->addItem(QString::fromStdString(ttime) + " " + QString::fromStdString(this->mw_act_clients.active_client_host_name) + ": " + QString::fromStdString(this->mw_tcp_socket->message.back()));
            this->mw_tcp_socket->message.pop_back();
        }
    }
}

void MainWindow::active_clients() {
    while(true) {
        for(int i = 0; i < this->mw_act_clients.online_clients.size(); i++) {
            QString username = QString::fromStdString(this->mw_act_clients.online_clients.at(i).first);
            QList<QListWidgetItem *> items = ui->activeClientsList->findItems(username, Qt::MatchExactly);
            if(items.size() == 0) {
                ui->activeClientsList->addItem(username);
            }
        }
        sleep(10);
    }
}

