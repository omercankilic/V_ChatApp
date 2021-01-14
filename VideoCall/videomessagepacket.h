#ifndef VIDEOMESSAGEPACKET_H
#define VIDEOMESSAGEPACKET_H
#include "FFmpeg_Headers.h"


class VideoMessagePacket
{
    public:
        VideoMessagePacket(AVPacket enc_packet);
    //private:
        long long paket_numbe    = 0;
        long long gonderme_zaman = 0;
        int packet_size          = 0;
        AVPacket pkt;
        
};

#endif // VIDEOMESSAGEPACKET_H
