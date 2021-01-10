#ifndef SENDER_H
#define SENDER_H
#include "FFmpeg_Headers.h"
#include <QObject>
#include <string>
#include <iostream>

using namespace std;
namespace vc{
    enum SenderHatalar{
        FORMAT_CONTEXT_OLUSTURULAMADI,
        STREAM_INFO_BULUNAMADI,
        AVCODEC_BULUNAMADI,
        S_CODEC_CTX_OLUSTURULAMADI,
    };
    
    class Sender:public QObject
    {
            Q_OBJECT
        public:
            Sender();
            Sender(string target_url,string target_port,camera_type active_cam);
            friend class VideoCall;
            
       private:
            
            int exit_flag = false;
            //FFmpeg parameters
            AVCodec *s_codec;
            AVCodecContext  *s_codec_ctx;
            AVFormatContext *s_format_ctx;
            
            //Output format variables
            AVCodec *out_codec;
            AVCodecContext *out_codec_ctx;
            AVFormatContext *out_format_ctx;
            int audio_stream_index;
            int video_stream_index;
            
            
            //target parameters
            string target_url;
            string target_port;
            string file_name;
            
            //sender parameters
            camera_type active_cam;
            
            
            //ffmpeg functions
            int in_format_ctx_set();
            int out_format_ctx_set();
            int encode_and_send(AVFrame* in_frame);
            int start_sending();
            int pause_sending();
            int stop_sending();
            
            bool is_paused = false;
            condition_variable cv_pause;
            mutex cv_pause_mut;
        signals:
            void video_paused();
            void video_stop();
            void video_continue();
            
    };
}

#endif // SENDER_H
