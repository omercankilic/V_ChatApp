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
    
            signals:
                void new_msg_online (QString);
                void udp_respond_signal(QString);
                
                public:
                    Udp_Socket(std::string ip_addr,Active_Clients *act_c,std::string usr_name);
                    void packet_listen();
                    void socket_listen();
                    void send_message(std::string ip, uint8_t pkt[]);
                   
                    int broadcast_listen();
                    int send_discover_msg();
                    int send_respond_msg(char target_ip[]);


                private:

                    int port_number = 23000;
                    char *ip_assigned = nullptr;
                    Active_Clients *act_clients;
                    std::thread *udp_listen_th;
                    std::thread *udp_listen_broadcast_th;
                    char broadcast_ip[15] = "192.168.1.255";
                    std::string usr_name;
                    
                    


        };


}


#endif // UDP_SOCKET_H
