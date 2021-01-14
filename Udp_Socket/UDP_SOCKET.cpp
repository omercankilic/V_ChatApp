#include <Udp_Socket/UDP_SOCKET.h>


namespace Chat {

    Udp_Socket::Udp_Socket(std::string ip_addr,Active_Clients *act_c){
            act_clients = act_c;
            ip_assigned = new char[16];
            strcpy(ip_assigned,ip_addr.c_str());
            create_socket();
            udp_listen_th = new std::thread([this](){ socket_listen();});
    }

    void Udp_Socket::create_socket() {
        server_sockfd = socket(AF_INET,SOCK_DGRAM,0);
        setsockopt(server_sockfd,SOL_SOCKET,SO_REUSEADDR,0,sizeof(int));

        if(server_sockfd <0)
        {
            std::cout << "UDP socket creating error." << std::endl;
        }
        memset(&server_addr, 0, sizeof(server_addr));
        memset(&client_addr, 0, sizeof(client_addr));

        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr((const char*) ip_assigned);
        server_addr.sin_port = htons(port_number);

        if( 0 > bind(server_sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)))
        {
            std::cout << "UDP socket binding error." << std::endl;
        }
    }

    void Udp_Socket::socket_listen() {
        char tempdata[30];
        int received_size;
        socklen_t len = sizeof(client_addr);

        while (true) {
            // recv(server_sockfd, tempdata, 30, 0);
            received_size = recvfrom(server_sockfd, tempdata, 30, 0, (struct sockaddr *)&client_addr, &len);
            char client_ip[16];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, 16);
        }
    }

    void Udp_Socket::send_message(std::string ip, std::string msg) {
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

        sendto(sockfd, msg.c_str(), 30, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        close(sockfd);
    }


}
