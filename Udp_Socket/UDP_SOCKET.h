#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>
#include <MessagePacket.h>
#include <ActiveClients/Active_Clients.h>
#include <QObject>
#include "VideoCall/videomessagepacket.h"

namespace Chat{

        class Udp_Socket: public QObject{

                    Q_OBJECT

                public:
                    Udp_Socket(std::string ip_addr,Active_Clients *act_c);
                    void create_socket();
                    void socket_listen();
                    void send_message(std::string ip, VideoMessagePacket pkt);


                private:

                    struct sockaddr_in server_addr, client_addr;
                    int server_sockfd;
                    int port_number = 23000;
                    char *ip_assigned = nullptr;
                    Active_Clients *act_clients;
                    std::thread *udp_listen_th;


        };


}


#endif // UDP_SOCKET_H
