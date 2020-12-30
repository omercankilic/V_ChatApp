#ifndef ACTIVE_CLIENTS_H
#define ACTIVE_CLIENTS_H
#include <vector>
#include <deque>
#include<iostream>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

typedef std::pair<std::string,std::string> active_client;

namespace Chat{
 
        class Active_Clients{
                public:
                        struct sockaddr_in active_client_addr;
                        int active_client_sockfd = -1111;
                        std::string active_client_host_name;
                        std::string active_client_ip_addr;
                        
                        std::vector<active_client> online_clients;
                        
                        int delete_host(char* host_name){
                                std::string str_hostname(host_name);
                                std::vector<active_client>::iterator itr;
                                for( itr = online_clients.begin();itr<online_clients.end();itr++){
                                        if(str_hostname == itr->first){
                                               online_clients.erase(itr);
                                               return 1;
                                        }
                                }
                                return -1;
                        }
                  
                        
                        //returns -1 if given host is not active
                        //otherwise it returns  host index in vector
                        int is_active_client(char *host_name){
                                
                                std::string str_hostname(host_name);
                                int i = 0;
                                std::vector<active_client>::iterator itr;
                                for( itr = online_clients.begin();itr<online_clients.end();itr++){
                                        if(str_hostname == itr->first){
                                               online_clients.erase(itr);
                                               return i;
                                        }
                                        i++;
                                }
                                return -1;
                        }
                       
                        int add_to_list(char* host_n,char *host_i ){
                                        
                                std::string host_name(host_n);
                                std::string host_ip(host_i);
                                active_client new_client;
                                new_client.first = host_name;
                                new_client.second = host_ip;
                                online_clients.push_back(new_client); 
                                return 1;
                        }
        };
        
        
        
}
        



#endif //ACTIVE_CLIENTS_H
