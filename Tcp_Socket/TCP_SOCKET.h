#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H



#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#include <vector>
#include <cstring>
#include <string>



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
         
         
        class Tcp_Socket{
                     
                public:
                        Tcp_Socket(std::string ip_addr){
                                ip_assigned = new char[16];
                                strcpy(ip_assigned,ip_addr.c_str());
                                
                                //input_msg = new MessagePacket();
                        }
                        Tcp_Socket(){
                               // input_msg = new MessagePacket();
                        }
                        int create_socket(){
                             
                               server_sockfd = socket(AF_INET,SOCK_STREAM,0);
                               setsockopt(server_sockfd,SOL_SOCKET,SO_REUSEADDR,0,sizeof(int));
                               if(server_sockfd <0)
                               {
                                       return SOCKET_CREATING_ERROR;
                               }
                               bzero((char *)&server_addr,sizeof(server_addr));
                               
                               //Server socket descriptions
                               //AF_INET is for ip4 socket
                               server_addr.sin_family = AF_INET;
                               server_addr.sin_addr.s_addr = inet_addr((const char*)ip_assigned);
                               server_addr.sin_port = htons(port_number);
                               
                               // binding socket to a port and ip address
                               if( 0 > bind(server_sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)))
                               {
                                       return SOCKET_BINDING_ERROR;
                               }
                               
                        }
                        int send_message(std::string msg){
                                char cmsg[msg.length()+1];
                                strcpy(cmsg,msg.c_str());
                                Chat::MessagePacket *msg_packet = new MessagePacket((uint8_t)CONNECTION_MESSAGE,3000,0,cmsg);
                                char *msg_pkt = reinterpret_cast<char*>(msg_packet);
                                send(act_clients->active_client_sockfd,msg_pkt,3007,0);
                                return 1;        
                        }
                        int send_connection_request(std::string host_ip){
                                char chost_ip[host_ip.length()+1];
                                strcpy(chost_ip, host_ip.c_str());
                               
                                //Socket setup
                                struct sockaddr_in send_addr;
                                int temp_sock =socket(AF_INET,SOCK_STREAM,0);
                                send_addr.sin_family = AF_INET;
                                send_addr.sin_port   = htons(52000);
                                if(inet_pton(AF_INET,chost_ip,&send_addr.sin_addr)<=0){
                                    std::cout<<"host_ip and socket can not set"<<std::endl;    
                                    return -1;
                                }
                                
                                if(connect(temp_sock,(struct sockaddr *)&send_addr,sizeof(send_addr))<0){
                                        std::cout<<"Socket can not connect to client"<<host_ip<<std::endl;
                                        return -1;
                                }
                                std::string  usr_n = this->user_name;
                                char cuser[usr_n.length()+1];
                                strcpy(cuser,usr_n.c_str());
                                Chat::MessagePacket *conn_request_pack = new Chat::MessagePacket((uint8_t)CONNECTION_START,3000,0,cuser);
                                char* conn_req = reinterpret_cast<char*>(conn_request_pack);
                                send(temp_sock,conn_req,3007,0);
                                
                               // recv(temp_sock,conn_req,3007,0);
                               // conn_request_pack = reinterpret_cast<Chat::MessagePacket*>(conn_request_pack);
                               // if(conn_request_pack->packet_type == CONNECTION_ACCEPT){
                               //         active_client_sockfd = temp_sock;
                               //         is_connected = true;
                               // }else{
                               //         delete conn_request_pack;
                               // }
                                delete conn_request_pack;
                                return 0;
                                return 1;
                                
                        }
                  
                      
                       
                        int is_accepted_f(char *host_name,struct sockaddr_in &client){
                                char client_ip[16];
                               char answer;
                                
                                inet_ntop(AF_INET, &client.sin_addr, client_ip,16 );
                                std::cout<<host_name<<"("<<client_ip<<")"<<" requested a connection ? (Y / N)"<<std::endl;
                                std::cin>>answer; 
                        
                                if(answer == 'y' || answer == 'Y'){
                                         return ACCEPTED;
                                }else{
                                        return REJECTED;
                                }
                                
                        }
                        
        
                        
                        int socket_listen(){
                                if(listen(server_sockfd,SOMAXCONN) ==-1){
                                        return SOCKET_LISTEN_ERROR;
                                }
                                int res;
                                active_client temp_client;
                                char tempdata[3007];
                                while(true){
                                        int temp_client_sockfd;
                                        struct sockaddr_in temp_client_addr;
                                        socklen_t temp_client_len= sizeof(temp_client_addr);
                                        temp_client_sockfd = accept(server_sockfd,(sockaddr *)&temp_client_addr,&(temp_client_len));
                                        if(temp_client_sockfd ==-1){
                                                std::cout<<"Problem with client connecting"<<std::endl;
                                        }
                                        
                                        if(recv(temp_client_sockfd,tempdata,3007,0) != -1){
                                                MessagePacket *temp_message_packet = reinterpret_cast<MessagePacket*>(tempdata);
                                                res = temp_message_packet->connection_manipulation();
                                                if(res == CONNECTION_START){
                                                        if(is_connected == false){
                                                                
                                                              if(ACCEPTED == is_accepted_f(temp_message_packet->packet_raw_data,temp_client_addr)){
                                                                      
                                                                        MessagePacket  *accept_packet = new MessagePacket(CONNECTION_ACCEPT,3007,0,ip_assigned);
                                                                        char *raw_data_send = reinterpret_cast<char*>(accept_packet);
                                                                        send(temp_client_sockfd,raw_data_send,3007, 0);
                                                                        is_connected = true;
                                                                        
                                                                        act_clients->active_client_sockfd = temp_client_sockfd;
                                                                        act_clients->active_client_addr = temp_client_addr;
                                                                        std::string temp_name(temp_message_packet->packet_raw_data);
                                                                        act_clients->active_client_host_name = temp_name;
                                                                        
                                                                        delete accept_packet;
                                                                        //close(temp_client_sockfd);
                                                              }else{
                                                                      MessagePacket  *accept_packet = new MessagePacket(CONNECTION_REFUSED,3007,0,ip_assigned);
                                                                      char *raw_data_send = reinterpret_cast<char*>(accept_packet);
                                                                      send(temp_client_sockfd,raw_data_send,3007, 0);
                                                                      close(temp_client_sockfd);
                                                              }
                                                        }else{
                                                                MessagePacket  *refused_packet = new MessagePacket(CONNECTION_REFUSED,3007,0,ip_assigned);
                                                                char *raw_data_send = reinterpret_cast<char*>(refused_packet);
                                                                send(temp_client_sockfd,raw_data_send,3007, 0);
                                                                close(temp_client_sockfd);
                                                                delete refused_packet;
                                                        }
                                                        delete temp_message_packet;
                                                }else if(res == CONNECTION_STOP){
                                                        //if I am listening this client right now I will detect it and 
                                                        if(temp_client_sockfd == act_clients->active_client_sockfd){
                                                                close(act_clients->active_client_sockfd);
                                                                is_connected = false;
                                                        }
                                                        
                                                        //close my current communication with temp_client_sock
                                                }else if(res == CONNECTION_MESSAGE){
                                                       // if(temp_client_sockfd == act_clients->active_client_sockfd){
                                                                std::cout<<" Message -> "<<temp_message_packet->packet_raw_data<<std::endl;
                                                        //}else{
                                                                delete temp_message_packet;
                                                                close(temp_client_sockfd);
                                                        //}
                                                }else{
                                                        
                                                        //send I am busy message to client and close sockfd
                                                        MessagePacket  *accept_packet = new MessagePacket(CONNECTION_REFUSED,3007,0,ip_assigned);
                                                        char *raw_data_send = reinterpret_cast<char*>(accept_packet);
                                                        send(temp_client_sockfd,raw_data_send,3007, 0);
                                                        close(temp_client_sockfd);
                                                }
                                        }else{
                                                close(temp_client_sockfd);
                                        }
                                       
                                                         
                                }
                                close(server_sockfd);
                        }
                private:
                        bool  is_connected = false;
                        int server_sockfd, active_client_sockfd;
                        uint16_t port_number = 52000;
                        char *ip_assigned = nullptr; 
                        struct sockaddr_in server_addr,  active_client_addr;
                        //MessagePacket *input_msg;           
                        Active_Clients *act_clients;
                        std::string user_name;
                                        
        };
}





#endif // TCP_SOCKET_H
