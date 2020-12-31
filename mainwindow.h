#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Tcp_Socket/TCP_SOCKET.h>
#include <Udp_Socket/UDP_SOCKET.h>
#include <ActiveClients/Active_Clients.h>
#include <thread>
#include <condition_variable>

using namespace Chat;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
                Q_OBJECT

        public:
                MainWindow(QWidget *parent = nullptr,std::string ip_n="192.168.1.6",std::string user_n="omercan");
                ~MainWindow();
                Tcp_Socket *mw_tcp_socket;
                Udp_Socket *mw_udp_socket;
                std::string ip_addres;
                std::string user_name;
                
        private slots:
                
        private:
                 Ui::MainWindow *ui;
                 std::thread *tcp_listen_th;
                 std::thread *udp_listen_th;
                 
                 
                 
};
#endif // MAINWINDOW_H
