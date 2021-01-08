#ifndef SENDER_H
#define SENDER_H

#include "FFmpeg_Headers.h"
#include <QObject>
#include <string>
#include <iostream>
using namespace std;
namespace vc{
    
    class Sender:public QObject
    {
            Q_OBJECT
        public:
            Sender(string target_url,string target_port);
            friend class VideoCall;
            
        private:
            AVCodec *codec;
            AVCodecContext *codec_ctx;
            AVFormatContext *format_ctx;
            string target_url;
            string target_port;
            
            void format_ctx_set();
            void create_decoder();
            void start_capture();
            void start_sending();
            void pause_sending();
            void stop_sending();
            
    };
}

#endif // SENDER_H
