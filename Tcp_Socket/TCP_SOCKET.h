#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H



#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Socket libraries
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#include <vector>
#include <cstring>
#include <string>
#include <thread>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>

#include <QObject>
#include <QMessageBox>
#include <QIcon>

#include "MessagePacket.h"
#include "ActiveClients/Active_Clients.h"

namespace Chat {

    enum TCP_SOCKET_ERRORS{
        SOCKET_CREATING_ERROR,
        SOCKET_BINDING_ERROR,
        SOCKET_LISTEN_ERROR,
        SOCKET_SUCCES,
        ACCEPTED,
        REJECTED,
    };

    enum THREAD_STATUS{
        THREAD_STOPPED,
    };

    enum GUI_LOGS{
        CLIENT_ADDED,

    };


    class Tcp_Socket:public QObject{
            
            Q_OBJECT
            
        public slots:
            void send_video_connection_request();
            
        signals:
            void new_msg_came         (QString);//Message type == MESSAGE
            void new_msg_online_tcp   (QString);//Message type == MESSAGE
            
            void new_msg_accepted_clr();
            void connectionStart(QString, QString);
            void connectionNotification(QString);
            void new_msg_deactive();
            void new_msg_activate(QString,int);
            
            void new_msg_video_call_response(bool is_accepted);
            void new_msg_video_call_request(bool close_or_open);
            
        public:
            bool exit_flag = false;
            std::vector<std::string> message;
            Tcp_Socket(std::string username, std::string ip_addr,Active_Clients *act_c);
            bool  is_connected = false;
            std::string user_name;
            char *ip_assigned = nullptr;
            
            //Api Functions
            int create_socket();
            void send_message(std::string msg,std::string ip, int conn_type);
            void send_video_message(uint8_t vc_msg_type);
            int send_connection_request(std::string host_ip);
            void is_accepted_f(char *host_name, struct sockaddr_in &client);
            int socket_listen();
            int set_discover_ip_list();       
            int send_discover_msg();
            Active_Clients *act_clients;

        private:
            std::thread *info_thread=nullptr;
            int server_sockfd, active_client_sockfd;
            uint16_t port_number = 52000;
            struct sockaddr_in server_addr,  active_client_addr;
            //MessagePacket *input_msg;
            std::vector<std::string> discover_ip_list;
            std::thread *tcp_listen_th;
    };
}





#endif // TCP_SOCKET_H
