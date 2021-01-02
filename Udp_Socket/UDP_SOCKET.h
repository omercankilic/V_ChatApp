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

namespace Chat{

        class Udp_Socket{

                public:
                        Udp_Socket(std::string ip_addr,Active_Clients *act_c){
                                act_clients = act_c;
                                ip_assigned = new char[16];
                                strcpy(ip_assigned,ip_addr.c_str());
                                //udp_listen_th = new thread([this](){
                                //        start_listen();'
                                //});
                        }
                        Udp_Socket(){

                        }
                        int socket_create(){

                                udp_sockfd= socket(AF_INET, SOCK_DGRAM, 0);

                                setsockopt(udp_sockfd,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof(int));
                                servaddr.sin_family    = AF_INET; // IPv4
                                servaddr.sin_addr.s_addr = inet_addr((const char*)ip_assigned);;
                                servaddr.sin_port = htons(udp_listen_port);

                                bind(udp_sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));

                                return 1;
                        }


                        int udp_conn_operation(int conn_tp,struct sockaddr_in client,char *host_name){

                                if(conn_tp ==CONNECTION_ONLINE){
                                        char client_ip[16];
                                        inet_ntop(AF_INET, &client.sin_addr, client_ip,16 );
                                        act_clients->add_to_list(host_name,client_ip);
                                }else if(conn_tp == CONNECTION_OFFLINE){

                                        std::string str_host_name(host_name);
                                        if(str_host_name== act_clients->active_client_host_name){
                                                close(act_clients->active_client_sockfd);
                                                act_clients->active_client_sockfd = -1111;
                                        }
                                        act_clients->delete_host(host_name);

                                }
                                return 1;
                        }

                        int start_listen(){
                                socklen_t clie_size = sizeof(cliaddr);
                                while(false == close_flag){
                                        char *buff = new char[3007];
                                        int nb_bytes = recvfrom(udp_sockfd,buff,3007,0,(struct sockaddr *)&cliaddr,&clie_size);
                                        if(nb_bytes == -1){
                                                std::cout<<"UDP_SOCKET RECVFROM ERROR"<<std::endl;
                                                delete []buff;
                                                continue;
                                        }
                                       MessagePacket *temp_msg = reinterpret_cast<MessagePacket*>(buff);
                                       int conn_type = temp_msg->connection_manipulation();
                                       udp_conn_operation(conn_type,cliaddr,temp_msg->packet_raw_data);
                                       delete temp_msg;
                                       delete buff;

                                }
                                return 1;

                        }

                private:

                        struct sockaddr_in servaddr, cliaddr;
                        int udp_sockfd;
                        const int udp_listen_port = 23000;
                        char *ip_assigned = nullptr;
                        int broadcast = 1;
                        bool close_flag = false;
                        Active_Clients *act_clients;
                        std::thread *udp_listen_th;


        };


}


#endif // UDP_SOCKET_H
