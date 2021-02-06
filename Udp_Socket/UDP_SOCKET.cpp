#include <Udp_Socket/UDP_SOCKET.h>


namespace Chat {
    
    Udp_Socket::Udp_Socket(std::string ip_addr,Active_Clients *act_c,std::string usr_name){
        this->usr_name = usr_name;
        act_clients = act_c;
        ip_assigned = new char[16];
        strcpy(ip_assigned,ip_addr.c_str());
        //here we have to determine broadcast ip address 
        //We have to add 255 to current ip_assigned
        
        udp_listen_broadcast_th = new::thread([this](){broadcast_listen();});
        //udp_listen_th = new std::thread([this](){ packet_listen();});
        
        
        send_discover_msg();
    }
    
    //This socket is for video packets
    int Udp_Socket::packet_listen() {
        struct sockaddr_in server_addr, client_addr;
        int server_sockfd;
        server_sockfd = socket(AF_INET,SOCK_DGRAM,0);
        if(server_sockfd <0)
        {
            std::cout << "UDP socket creating error." << std::endl;
            close(server_sockfd);
            return 0;
        }
        memset(&server_addr, 0, sizeof(server_addr));
        memset(&client_addr, 0, sizeof(client_addr));
        
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr((const char*) ip_assigned);
        server_addr.sin_port = htons(port_number);
        
        if( 0 > bind(server_sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)))
        {
            std::cout << "UDP socket binding error." << std::endl;
            close(server_sockfd);
            return 0;
        }
        uint8_t tempdata[100];
        int received_size;
        socklen_t len = sizeof(client_addr);
        
        while (true) {
            // recv(server_sockfd, tempdata, 30, 0);
            received_size = recvfrom(server_sockfd, tempdata, 100, 0, (struct sockaddr *)&client_addr, &len);
            char client_ip[16];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, 16);
            cout<<"client ip : "<<client_ip<<endl;
            cout<<"tempdata : "<<tempdata<<endl; 
            
        }
        
        return 0;
    }
    
    void Udp_Socket::send_message(std::string ip, uint8_t pkt[]) {
        
        int sockfd;
        struct sockaddr_in servaddr;
        
        if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
            std::cout << "UDP socket creating error(sending message)." << std::endl;
        }
        
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port_number);
        
        if(inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr)<=0){
            std::cout << "Can not inet_pton." << std::endl;
        }
        
        uint64_t pkt_size = sizeof(pkt)/sizeof(uint8_t);
        sendto(sockfd, pkt, pkt_size, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        
        close(sockfd);
    }
    
    int Udp_Socket::broadcast_listen()
    {
        int sockfd;
        struct sockaddr_in server_addr, client_addr;
        
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        int broadcastEnable = 1;
        int ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
        if (ret) {
            cout<<"Error in setting Broadcast option" << endl;
            close(sockfd);
            return 0;
        }
        setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,0,sizeof(int));
        memset(&server_addr, 0, sizeof(server_addr));
        memset(&client_addr, 0, sizeof(client_addr));
        
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr((const char*) broadcast_ip);
        server_addr.sin_port = htons(broadcast_port);
        
        bind(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr));
        
        char tempdata[3007];
        int received_size;
        socklen_t len = sizeof(client_addr);
        
        while(true) {
            received_size = recvfrom(sockfd, tempdata, 3007, 0, (struct sockaddr *)&client_addr, &len);
            char client_ip[15];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, 15);
            MessagePacket *temp_message_pkt;
            temp_message_pkt = reinterpret_cast<MessagePacket*>(tempdata);
            if(temp_message_pkt->packet_type ==CONNECTION_ONLINE){
                active_client new_client;
                new_client.first = temp_message_pkt->packet_raw_data;
                new_client.second= client_ip;
                if (temp_message_pkt->packet_raw_data != usr_name) {
                    act_clients->online_clients.push_back(new_client);
                    emit new_msg_online((QString)temp_message_pkt->packet_raw_data);
                    emit udp_respond_signal((QString::fromStdString((string(client_ip)))));
                }else {
                    cout << "ayni" << endl;
                }
            }
        }
        return 0;
    }
    
    int Udp_Socket::send_discover_msg()
    {
        int sockfd;
        struct sockaddr_in servaddr;
        
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        int broadcastEnable = 1;
        int ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
        if (ret) {
            cout<<"Error in setting Broadcast option" << endl;
            close(sockfd);
            return 0;
        }
        
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(broadcast_port);
        servaddr.sin_addr.s_addr = inet_addr((const char*) broadcast_ip);
        
        MessagePacket *temp_packet = new MessagePacket((uint8_t)CONNECTION_ONLINE,3000,0,(char*)usr_name.c_str());
        char* packet = reinterpret_cast<char*>(temp_packet);
        for(int i = 0;i<3;i++){
            sendto(sockfd, packet, 3007, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        }
        
        delete temp_packet;
        packet = nullptr;
        close(sockfd);
        
        cout << "gonderildi" << endl;
        return 0;
    }
    
    
}



/*
#include <Udp_Socket/UDP_SOCKET.h>


namespace Chat {

    Udp_Socket::Udp_Socket(std::string ip_addr,Active_Clients *act_c,std::string usr_name){
        this->usr_name = usr_name;
        act_clients = act_c;
        ip_assigned = new char[16];
        strcpy(ip_assigned,ip_addr.c_str());
        //here we have to determine broadcast ip address 
        //We have to add 255 to current ip_assigned
        
        udp_listen_broadcast_th = new std::thread([this](){broadcast_listen();});
        //udp_listen_th           = new std::thread([this](){ packet_listen();});
        //send_discover_msg();
    }
    
    void Udp_Socket::send_message(std::string ip, uint8_t pkt[20000]) {
    
        cout<<"size pkt : "<<sizeof(pkt)/sizeof(uint8_t)<<endl;
        int sockfd;
        struct sockaddr_in servaddr;
        
        if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
            std::cout << "UDP socket creating error(sending message)." << std::endl;
        }
        
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port_number);
        servaddr.sin_addr.s_addr = inet_addr((const char*)"192.168.1.7");
        
       // if(inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr)<=0){
       //     std::cout << "Can not inet_pton." << std::endl;
       // }
       
        uint64_t pkt_size = sizeof(pkt)/sizeof(uint8_t);
        sendto(sockfd, pkt, pkt_size, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        
        close(sockfd);
    }
    
    int Udp_Socket::broadcast_listen()
    {
        struct sockaddr_in server_addr, client_addr;
        int server_sockfd;
        int ret;
        server_sockfd = socket(AF_INET,SOCK_DGRAM,0);
        int yes = 1;
        //ret = setsockopt(server_sockfd,SOL_SOCKET,SO_BROADCAST,(char*)&yes,sizeof(int));
        
        if(server_sockfd <0)
        {
            std::cout << "UDP socket creating error." << std::endl;
        }
        memset(&server_addr, 0, sizeof(server_addr));
        memset(&client_addr, 0, sizeof(client_addr));
        
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr((const char*) broadcast_ip);
        server_addr.sin_port = htons(port_number);
        
        if( 0 > bind(server_sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)))
        {
            std::cout << "UDP socket binding error." << std::endl;
        }
        char tempdata[3007];
        int received_size;
        socklen_t len = sizeof(client_addr);
        
        while (true) {
            received_size = recvfrom(server_sockfd, tempdata, 3007, 0, (struct sockaddr *)&client_addr, &len);
            char client_ip[15];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, 15);
            cout<<"client ip : "<<client_ip<<endl;
            cout<<"tempdata : "<<tempdata<<endl; 
            MessagePacket *temp_message_pkt;
            temp_message_pkt = reinterpret_cast<MessagePacket*>(tempdata);
            cout<<temp_message_pkt->packet_raw_data<<endl;
            if(temp_message_pkt->packet_type ==CONNECTION_ONLINE){
                active_client new_client;
                new_client.first = temp_message_pkt->packet_raw_data;
                new_client.second= client_ip;
                act_clients->online_clients.push_back(new_client);
                
                //emit new_msg_online((QString)temp_message_pkt->packet_raw_data);
                //emit udp_respond_signal((QString::fromStdString((string(client_ip)))));
                //send_respond_msg(client_ip);
                close(server_sockfd);  
            }
            
        }
        
    }
    
    int Udp_Socket::send_discover_msg()
    {
        int sock;
        int yes = 1;
        struct  sockaddr_in broadcast_addr;
        int addr_len;
        int ret;
        sock = socket(AF_INET,SOCK_DGRAM,0);
        if(sock<0){
            cout<<"send_discover_msg udp socket creation error"<<endl;
            return -1;
        }
        
        ret = setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(char *)&yes,sizeof(yes));
        if(ret==-1){
            cout<<"udp send discover msg setsockopt error"<<endl;
            return 0;
        }
        addr_len = sizeof(struct  sockaddr_in);
        
        memset((void*)&broadcast_addr,0,addr_len);
        broadcast_addr.sin_family = AF_INET;
        broadcast_addr.sin_addr.s_addr = inet_addr((const char*)broadcast_ip);
        broadcast_addr.sin_port = htons(port_number);
        MessagePacket *temp_packet = new MessagePacket((uint8_t)CONNECTION_ONLINE,3000,0,(char*)usr_name.c_str());
        char *packet = reinterpret_cast<char*>(temp_packet);
        for(int i = 0;i<3;i++){
            sendto(sock,packet,3007,0,(struct sockaddr*) &broadcast_addr,addr_len);
        }
        cout<<"udp broadcast discover send "<<endl;
        close(sock);
        return 0;
        
    }
    
    
}
*/
