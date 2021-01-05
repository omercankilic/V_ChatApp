#include<Tcp_Socket/TCP_SOCKET.h>
//usr/bin/ld: /usr/local/lib/libavcodec.a(vc1dsp_mmx.o): relocation R_X86_64_PC32 against symbol `ff_pw_9' can not be used when making a shared object; recompile with -fPIC
//usr/bin/ld: final link failed: bad value
//collect2: error: ld returned 1 exit status
//make[2]: *** [modules/videoio/CMakeFiles/opencv_videoio.dir/build.make:300: lib/libopencv_videoio.so.4.5.1] Error 1
//make[1]: *** [CMakeFiles/Makefile2:5761: modules/videoio/CMakeFiles/opencv_videoio.dir/all] Error 2
//make[1]: *** Waiting for unfinished jobs....

namespace Chat{
    
    Tcp_Socket::Tcp_Socket(std::string username, std::string ip_addr,Active_Clients *act_c){
        this->user_name = username;
        ip_assigned = new char[ 16];
        strcpy(ip_assigned,ip_addr.c_str());
        create_socket();
        act_clients = act_c;
        tcp_listen_th = new std::thread( [this](){ socket_listen();});
        set_discover_ip_list();
        send_discover_msg();
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
                        is_accepted_f(temp_message_packet->packet_raw_data,temp_client_addr);
                        close(temp_client_sockfd);
                        //if( ACCEPTED== is_accepted_f(temp_message_packet->packet_raw_data,temp_client_addr)){
                        //
                        //    close(temp_client_sockfd);
                        //    send_message(ip_assigned, client_ip, CONNECTION_ACCEPT);
                        //    is_connected = true;
                        //
                        //    act_clients->active_client_sockfd = temp_client_sockfd;
                        //    act_clients->active_client_addr = temp_client_addr;
                        //    std::string temp_name(temp_message_packet->packet_raw_data);
                        //    act_clients->active_client_host_name = temp_name;
                        //                            
                        //}else{
                        //    send_message(ip_assigned, client_ip, CONNECTION_REFUSED);
                        //    close(temp_client_sockfd);
                        //}
                    }else{
                        send_message(ip_assigned, client_ip, CONNECTION_REFUSED);
                        close(temp_client_sockfd);
                    }
                    delete temp_message_packet;
                }else if(res == CONNECTION_STOP){
    
                    if( this->act_clients->active_client_ip_addr == std::string(client_ip)){
                        
                        is_connected = false;
                        act_clients->active_client_ip_addr  ="";
                        act_clients->active_client_host_name= "";                        
                    }
                    close(temp_client_sockfd);
                }else if(res == CONNECTION_MESSAGE){
                    if(this->is_connected == true){
                        emit new_msg_came((QString)temp_message_packet->packet_raw_data);
                        temp_message_packet = nullptr;
                    }
                    close(temp_client_sockfd);
                    
                }else if (res == CONNECTION_ONLINE){
    
                    std::cout<<(uint16_t)(temp_message_packet->packet_type)<<std::endl;
                    active_client new_client;
                    new_client.first = temp_message_packet->packet_raw_data;
                    new_client.second= client_ip;
                    act_clients->online_clients.push_back(new_client);
                    emit new_msg_online((QString)temp_message_packet->packet_raw_data);
                    char usr[30];
                    strcpy(usr, user_name.c_str());
                    send_message(usr, client_ip, CONNECTION_RESPOND);
                    close(temp_client_sockfd);  
                }else if (res == (uint8_t)CONNECTION_RESPOND){
                    std::cout<<(uint16_t)(temp_message_packet->packet_type)<<std::endl;
                    active_client new_client;
                    new_client.first = temp_message_packet->packet_raw_data;
                    new_client.second= client_ip;
                    act_clients->online_clients.push_back(new_client);
                    emit new_msg_online((QString)temp_message_packet->packet_raw_data);
                    close(temp_client_sockfd);
    
                }else if(res == (uint8_t)CONNECTION_ACCEPT){
                    
                    if(false == this->is_connected){
                        
                        this->act_clients->active_client_host_name = temp_message_packet->packet_raw_data;
                        this->act_clients->active_client_sockfd    = temp_client_sockfd;
                        this->act_clients->active_client_ip_addr   = client_ip;
                        is_connected = true;
                        
                        std::thread *t_info_thread = new std::thread([=](){
                            QMessageBox msg_box;
                            QString info_msg = (QString)temp_message_packet->packet_raw_data+" accepted your connection request :) ";           
                            msg_box.setWindowTitle("Connection Information");
                            msg_box.setText(info_msg);
                            msg_box.setStandardButtons(QMessageBox::Close);
                            msg_box.setDefaultButton(QMessageBox::Close);
                            msg_box.exec();
                        });
                        t_info_thread->detach();
                        emit this->new_msg_accepted_clr();
                        
                    }
                    close(temp_client_sockfd);
                }else if(res == (uint8_t)CONNECTION_REFUSED){
                    std::thread *t_info_thread = new std::thread([=](){
                        QMessageBox msg_box;
                        QString info_msg = (QString)temp_message_packet->packet_raw_data+" REFUSED your connection request :( ";           
                        msg_box.setWindowTitle("Connection Information");
                        msg_box.setText(info_msg);
                        msg_box.setStandardButtons(QMessageBox::Close);
                        msg_box.setDefaultButton(QMessageBox::Close);
                        msg_box.exec();
                    });
                    t_info_thread->detach();
                    close(temp_client_sockfd);
                    
                }else{
    
                    send_message(ip_assigned, client_ip, CONNECTION_REFUSED);
                    close(temp_client_sockfd);
                }
            }else{
                close(temp_client_sockfd);
            }
    
        }
        close(server_sockfd);
    }
    
    int Tcp_Socket::send_discover_msg(){
        long arg;
        struct sockaddr_in client_to;
        int temp_sock = socket(AF_INET,SOCK_STREAM,0);
        client_to.sin_family = AF_INET;
        client_to.sin_port   = htons(52000);
        char arr[30];
        strcpy(arr, user_name.c_str());

        for(int i=0;i<(int)discover_ip_list.size();i++){
            char ip[16];
            strcpy(ip, discover_ip_list.at(i).c_str());
            if(inet_pton(AF_INET,ip,&client_to.sin_addr)<=0){
                std::cout<<"Can not inet_pton : "<<discover_ip_list.at(i)<<std::endl;
            }
            //if( (arg = fcntl(temp_sock, F_GETFL, NULL)) < 0) {
            //     fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
            //  }
            //
            //  arg |= O_NONBLOCK;
            //  if( fcntl(temp_sock, F_SETFL, arg) < 0) {
            //     fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
            // }

            if(::connect(temp_sock,(struct sockaddr *)&client_to,sizeof(client_to)) <0){
                std::cout<<"Can not connect to : "<<discover_ip_list.at(i)<<std::endl;
            }

            MessagePacket *temp_packet = new MessagePacket((uint8_t)CONNECTION_ONLINE,3000,0,arr);
            char *packet = reinterpret_cast<char*>(temp_packet);
            send(temp_sock,packet,3007,0);
            delete temp_packet;
        }
        close(temp_sock);
        return 1;
    }
    
    int Tcp_Socket::set_discover_ip_list(){
        char cmd[136];
        strcpy(cmd, "echo \"25.114.184.185\" > ip.txt");
        //strcpy(cmd,"s=192.168.1; for i in $(seq 0 254) ; do ( ping -c 1 -w 1 $s.$i 1>/dev/null 2>&1 && printf \\\"$s.$i \\\" ) & done | sed 's/\"/\\n/g' > ip.txt");
        //std::cout<<"cmd : "<<cmd<<std::endl;
        system(cmd);
        std::ifstream infile("ip.txt");
        std::string line;
        while(std::getline(infile,line)){
            if(line.length() != 0){
                discover_ip_list.push_back(line);
            }
        }

        return 1;
    }
    
    int Tcp_Socket::send_message(std::string msg, std::string ip, int conn_type){
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
        return 1;
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
    
    int Tcp_Socket::is_accepted_f(char *host_name, struct sockaddr_in &client){
        
        char client_ip[16];

        inet_ntop(AF_INET,&client.sin_addr,client_ip,16);


        std::thread *t_info_thread = new std::thread([&](){
            //QMessageBox msg_box;
            //QString info_msg = (QString)client_name+" want to talk to you? Yes/No ?";           
            //msg_box.setWindowTitle("Connection Information");
            //msg_box.setText(info_msg);
            //msw_box.setStandardButtons(QMessageBox::Yes);
            //msg_box.setDefaultButton(QMessageBox::Yes);
            //msw_box.setStandardButtons(QMessageBox::No);
            //msg_box.setDefaultButton(QMessageBox::No);
            //
            //if(QMessageBox::Yes ==  msg_box.exec()){
            //    send_message(this->user_name,ip_address,CONNECTION_ACCEPT);
            //}else{
            //    send_message(this->user_name,ip_address,CONNECTION_REFUSED);
            //}
            QString que = QString::fromStdString(std::string(host_name)) + " wants to talk to you ? (Yes/No)?";
            if (QMessageBox::Yes == QMessageBox(QMessageBox::Information, "title", que, QMessageBox::Yes|QMessageBox::No).exec()) 
            {
                if(false == this->is_connected){
                    send_message(this->user_name,std::string(client_ip),CONNECTION_ACCEPT);
                }else{
                    send_message(this->user_name,std::string(client_ip),CONNECTION_REFUSED);
                }
            }else{
                send_message(this->user_name,std::string(client_ip),CONNECTION_REFUSED);
            }
            
        });
        t_info_thread->detach();
        return 1;
    }
   
    
    
}
