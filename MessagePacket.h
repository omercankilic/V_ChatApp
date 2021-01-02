#ifndef MESSAGEPACKET_H
#define MESSAGEPACKET_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
namespace Chat {    
        enum PACKET_TYPES{
                CONNECTION_START,
                CONNECTION_STOP,
                CONNECTION_ONLINE,              //these type of messages will be send via broadcast UDP 
                CONNECTION_OFFLINE,            // these type of messages will be send via broadcast UDP
                CONNECTION_MESSAGE,
                CONNECTION_ACCEPT,
                CONNECTION_REFUSED,
                CONNECTION_RESPOND,
                UNDEFINED,
        };
        
        class MessagePacket{
                public:
                        uint8_t                      packet_type;              // 1byte
                        uint16_t                   packet_raw_data_size;//2 byte
                        uint32_t                   packet_timestamp; // 4 bytes
                        char                        packet_raw_data[3000]  ; // 3000 bytes for string message
                        //MessagePacket constructor//
                        MessagePacket(const uint8_t type,
                                                      const uint16_t raw_data_size,
                                                      const uint8_t time_stamp,char raw_data[] ):
                                                      packet_type((uint8_t)type),
                                                      packet_raw_data_size(raw_data_size),
                                                      packet_timestamp(time_stamp)
                                                      {
                                                        memset(&packet_raw_data_size,0,3000);
                                                        strcpy(packet_raw_data,raw_data);
                                                      }
                        MessagePacket(){}
                        
                        ~MessagePacket(){
                                
                        }
                        
                        int connection_manipulation(){
                              
                                if(packet_type == CONNECTION_ONLINE){
                                        // we will add the client to the active clients list
                                        return CONNECTION_ONLINE;
                                }else if(packet_type == CONNECTION_START){
                                        // this will be handled in our socket_listen func
                                        return CONNECTION_START;
                                }else if(packet_type == CONNECTION_STOP){
                                        // this will be handled in our socket_listen func
                                        return CONNECTION_STOP;
                                }else if(packet_type ==CONNECTION_OFFLINE){
                                        //we will remove the client from the active clients list
                                        return CONNECTION_OFFLINE;
                                }else if(packet_type == CONNECTION_MESSAGE){
                                        return CONNECTION_MESSAGE;
                                }
                                
                                return UNDEFINED;
                       
                        }
                        
        };

}


#endif // MESSAGEPACKET_H
