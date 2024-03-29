
#include<Tcp_Socket/TCP_SOCKET.h>
//usr/bin/ld: /usr/local/lib/libavcodec.a(vc1dsp_mmx.o): relocation R_X86_64_PC32 against symbol `ff_pw_9' can not be used when making a shared object; recompile with -fPIC
//usr/bin/ld: final link failed: bad value
//collect2: error: ld returned 1 exit status
//make[2]: *** [modules/videoio/CMakeFiles/opencv_videoio.dir/build.make:300: lib/libopencv_videoio.so.4.5.1] Error 1
//make[1]: *** [CMakeFiles/Makefile2:5761: modules/videoio/CMakeFiles/opencv_videoio.dir/all] Error 2
//make[1]: *** Waiting for unfinished jobs....

namespace Chat{
    
    void Tcp_Socket::send_video_connection_request()
    {
        this->send_message("",this->act_clients->active_client_ip_addr,CONNECTION_VIDEO_START);
    }
    
    Tcp_Socket::Tcp_Socket(std::string username, std::string ip_addr,Active_Clients *act_c){
        this->user_name = username;
        ip_assigned = new char[16];
        strcpy(ip_assigned,ip_addr.c_str());
        create_socket();
        act_clients = act_c;
        tcp_listen_th = new std::thread( [this](){ socket_listen();});
        //set_discover_ip_list();
        //send_discover_msg();
    }
    
    int Tcp_Socket::create_socket(){
        
        server_sockfd = socket(AF_INET,SOCK_STREAM,0);
        setsockopt(server_sockfd,SOL_SOCKET,SO_REUSEADDR,0,sizeof(int));
        
        if(server_sockfd <0)
        {
            return SOCKET_CREATING_ERROR;
        }
        bzero((char *)&server_addr,sizeof(server_addr));
        
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr((const char*)ip_assigned);
        server_addr.sin_port = htons(port_number);
        
        // binding socket to a port and ip address
        if( 0 > bind(server_sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)))
        {
            return SOCKET_BINDING_ERROR;
        }
        return 0;
    }
    
    int Tcp_Socket::socket_listen(){
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
            
            char client_ip[16];
            inet_ntop(AF_INET, &temp_client_addr.sin_addr, client_ip,16 );
            
            if(recv(temp_client_sockfd,tempdata,3007,0) != -1){
                MessagePacket *temp_message_packet = reinterpret_cast<MessagePacket*>(tempdata);
                res = temp_message_packet->connection_manipulation();
                if(res == CONNECTION_START){
                    if(is_connected == false){
                        
                        emit connectionStart((QString)temp_message_packet->packet_raw_data, client_ip);
                    }else{
                        send_message(this->user_name, client_ip, CONNECTION_REFUSED);
                        
                    }
                    close(temp_client_sockfd);
                    temp_message_packet = nullptr;
                }else if(res == CONNECTION_STOP){
                    
                    if( this->act_clients->active_client_ip_addr == std::string(client_ip)){
                        
                        is_connected = false;
                        act_clients->active_client_ip_addr  ="";
                        act_clients->active_client_host_name= "";
                        emit connectionNotification((QString)temp_message_packet->packet_raw_data + " stopped connection.");
                    }
                    close(temp_client_sockfd);
                }else if(res == CONNECTION_MESSAGE){
                    if(this->is_connected == true){
                        emit new_msg_came((QString)temp_message_packet->packet_raw_data);
                        temp_message_packet = nullptr;
                    }
                    close(temp_client_sockfd);
                    
                }else if (res == (uint8_t)CONNECTION_RESPOND){
                    std::cout<<(uint16_t)(temp_message_packet->packet_type)<<std::endl;
                    active_client new_client;
                    new_client.first = temp_message_packet->packet_raw_data;
                    new_client.second= client_ip;
                    act_clients->online_clients.push_back(new_client);
                    emit new_msg_online_tcp((QString)temp_message_packet->packet_raw_data);
                    close(temp_client_sockfd);
                    
                }else if(res == (uint8_t)CONNECTION_ACCEPT){
                    
                    if(false == this->is_connected){
                        
                        this->act_clients->active_client_host_name = temp_message_packet->packet_raw_data;
                        this->act_clients->active_client_sockfd    = temp_client_sockfd;
                        this->act_clients->active_client_ip_addr   = client_ip;
                        is_connected = true;
                        
                        emit connectionNotification((QString)temp_message_packet->packet_raw_data + " accepted your connection request.");
                        emit this->new_msg_accepted_clr();
                        
                    }
                    close(temp_client_sockfd);
                }else if(res == (uint8_t)CONNECTION_REFUSED){
                    emit connectionNotification((QString)temp_message_packet->packet_raw_data + " refused your connection request.");
                    close(temp_client_sockfd);
                    
                }else if(res == (uint8_t)CONNECTION_VIDEO_START){
                    std::cout << "buradaaaa" << std::endl;
                    emit this->new_msg_video_call_request(true);
                }else if(res == (uint8_t)CONNECTION_VIDEO_STOP){
                    emit this->new_msg_video_call_request(false);
                }else if(res == (uint8_t)CONNECTION_VIDEO_ACCEPTED){
                    emit this->new_msg_video_call_response(true);
                }else if(res == (uint8_t)CONNECTION_VIDEO_REFUSED){
                    emit this->new_msg_video_call_response(false);
                }else{
                    
                    send_message(ip_assigned, client_ip, CONNECTION_REFUSED);
                    close(temp_client_sockfd);
                }
            }else{
                close(temp_client_sockfd);
            }
            
        }
        close(server_sockfd);
        return 0;
    }
    
    
    void Tcp_Socket::send_message(std::string msg, std::string ip, int conn_type){
        struct sockaddr_in client_to;
        int temp_sock = socket(AF_INET,SOCK_STREAM,0);
        client_to.sin_family = AF_INET;
        client_to.sin_port   = htons(52000);
        
        char cip[ip.length()+1];
        strcpy(cip, ip.c_str());
        
        char cmsg[msg.length()+1];
        strcpy(cmsg,msg.c_str());
        
        if(inet_pton(AF_INET, cip, &client_to.sin_addr)<=0){
            std::cout<<"Can not inet_pton : "<< cip <<std::endl;
        }
        
        if(::connect(temp_sock,(struct sockaddr *)&client_to,sizeof(client_to)) <0){
            std::cout<<"Can not connect to : "<< cip <<std::endl;
        }
        
        Chat::MessagePacket *msg_packet = new MessagePacket((uint8_t)conn_type,3000,0,cmsg);
        char *msg_pkt = reinterpret_cast<char*>(msg_packet);
        send(temp_sock,msg_pkt,3007,0);
        close(temp_sock);
        delete msg_packet;
        msg_pkt = nullptr;
        //delete msg_packet;
        //return 1;
    }
    
    void Tcp_Socket::send_video_message(uint8_t vc_msg_type)
    {
        this->send_message(this->user_name,this->act_clients->active_client_ip_addr,vc_msg_type);
    }
    
    int Tcp_Socket::send_connection_request(std::string host_ip){
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
        
        if(::connect(temp_sock,(struct sockaddr *)&send_addr,sizeof(send_addr))<0){
            std::cout<<"Socket can not connect to client"<<host_ip<<std::endl;
            return -1;
        }
        std::string  usr_n = this->user_name;
        char cuser[usr_n.length()+1];
        strcpy(cuser,usr_n.c_str());
        Chat::MessagePacket *conn_request_pack = new Chat::MessagePacket((uint8_t)CONNECTION_START,3000,0,cuser);
        char* conn_req = reinterpret_cast<char*>(conn_request_pack);
        send(temp_sock,conn_req,3007,0);
        
        close(temp_sock);
        delete conn_request_pack;
        return 1;
        
    }
    
}

