#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Tcp_Socket/TCP_SOCKET.h>
#include <Udp_Socket/UDP_SOCKET.h>
#include <ActiveClients/Active_Clients.h>
#include <thread>
#include <condition_variable>
#include <QListWidgetItem>
#include "connectionstartdialog.h"
#include "connectionnotificationdialog.h"
#include "VideoCall/videocall.h"

using namespace Chat;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
        Q_OBJECT
        
    public:
        MainWindow(QWidget *parent = nullptr,std::string ip_n="192.168.1.6",std::string user_n="melih");
        ~MainWindow();
        Active_Clients  mw_act_clients;
        Tcp_Socket     *mw_tcp_socket;
        Udp_Socket     *mw_udp_socket;
        VideoCall      *mw_video_call;
        
    private slots:
        
        void on_sendMsgButton_clicked();
        void on_activeClientsList_itemDoubleClicked(QListWidgetItem *item);
        void msg_message_onl(QString);
        void msg_message_rcv(QString);
        void connectionStart(QString, QString);
        void connectionNotification(QString);
        
        void on_activateClientButton_clicked();
        void msg_box_clr();
        void on_stopClientButton_clicked();
        
        //Video slots
        void sendVideoStopMsg();
        void sendVideoPausedMsg();
        void sendVideoContinueMsg();
        void sendVideoStartMsg();
        
        void sendRespondMsg(QString target_ip);
        
        
        void on_videoButton_clicked();
        
    private:
        Ui::MainWindow *ui;
        std::string ip_addres;
        std::string user_name;
        std::thread *message_receive;
        std::thread *active_client;
        
        
        
};
#endif // MAINWINDOW_H
