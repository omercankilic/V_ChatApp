#ifndef VIDEOCALL_H
#define VIDEOCALL_H

#include <QWidget>
#include <iostream>
#include "VideoCall/receiver.h"
#include "VideoCall/sender.h"
#include <QCameraInfo>
#include <QMessageBox>

using namespace  std;
using namespace vc;
namespace Ui {
    class VideoCall;
}

class VideoCall : public QWidget
{
        Q_OBJECT
        
    public:
        explicit VideoCall(QWidget *parent = nullptr,string tar_url="192.168.1.7",int target_prt=23000,Udp_Socket *mw_udp_sock=nullptr);
        ~VideoCall();
        int detect_cameras();
        int video_stop();
        int video_start();
        Udp_Socket *vc_udp_sock;
        
        void packet_listen();
        std::thread *pkt_listen_th;
        
        
        
        //SENDER PROPERTIES START
        string file_name;
        int exit_flag = false;
        //FFmpeg parameters
        AVCodec *s_codec;
        AVCodecContext  *s_codec_ctx;
        AVFormatContext *s_format_ctx;
        
        //Output format variables
        AVCodec *out_codec;
        AVCodecContext *out_codec_ctx;
        AVFormatContext *out_format_ctx;
        AVIOContext     *output_io_context;
        
        int audio_stream_index;
        int video_stream_index;
        
        //invalid
        int out_format_ctx_set();
        
        //ffmpeg functions
        int output_format_ctx_set(string url);
        int in_format_ctx_set();
        int encode_and_send(AVFrame* in_frame);
        int start_sending();
        int pause_sending();
        int stop_sending();
        
        bool is_paused = false;
        condition_variable cv_pause;
        mutex cv_pause_mut;
        
        
        int start_encoding(AVFrame *frame);
        int set_encoder();
        AVCodec *encoder;
        AVCodecContext *encoder_ctx;
        
        
        AVCodec  *decoder;
        AVCodecContext *decoder_ctx;
        int set_decoder();
        int decode_and_show(AVPacket enc_pkt);
        int prepare_and_send_data(AVPacket &pkt);
        
        
        //SENDER PROPERTIES END
        
        
        Ui::VideoCall *ui;
        string target_url = "192.168.1.7";
        int target_port;
        Receiver *vc_receiver;
        Sender   *vc_sender;
        camera_type active_camera;
        vector<camera_type> available_cameras;
        bool is_connected = true;
        
    signals:
        void vc_stop();
        void vc_start();
        //void video_paused();
        //void video_stop();
        //void video_continue();
        
        
        
        
        
    public slots:
        void on_startVideo_clicked();
        void deactivated();
        void activated(QString trg_url,int trg_port);
};

#endif // VIDEOCALL_H
