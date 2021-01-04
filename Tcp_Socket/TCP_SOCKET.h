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
            
            
        signals:
            void new_msg_came   (QString);
            void new_msg_online (QString);
            
        public:
            std::vector<std::string> message;
            Tcp_Socket(std::string username, std::string ip_addr,Active_Clients *act_c);
            bool  is_connected = false;
            
            //Api Functions
            int create_socket();
            int send_message(std::string msg);
            int send_connection_request(std::string host_ip);
            int is_accepted_f(char *host_name, struct sockaddr_in &client);
            int socket_listen();
            int set_discover_ip_list();       
            int send_discover_msg();

        private:
            
            std::thread *info_thread=nullptr;
            int server_sockfd, active_client_sockfd;
            uint16_t port_number = 52000;
            char *ip_assigned = nullptr;
            struct sockaddr_in server_addr,  active_client_addr;
            //MessagePacket *input_msg;
            Active_Clients *act_clients;
            std::string user_name;
            std::vector<std::string> discover_ip_list;
            std::thread *tcp_listen_th;
    };
}





#endif // TCP_SOCKET_H
