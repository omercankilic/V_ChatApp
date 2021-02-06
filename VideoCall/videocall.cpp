#include "videocall.h"
#include "ui_videocall.h"
#include "video_funcs.h"
VideoCall::VideoCall(QWidget *parent,string tar_url,int target_prt,Udp_Socket *mw_udp_sock) :
    QWidget(parent),
    ui(new Ui::VideoCall),target_url(tar_url),target_port(target_prt)
{
    this->vc_udp_sock = mw_udp_sock;
    detect_cameras();
    active_camera.second="/dev/video0";
    ui->setupUi(this);
    //TODO new ui will be shown here and asked to select camera
}

VideoCall::~VideoCall()
{
    delete ui;
}

int VideoCall::detect_cameras()
{
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : cameras){
        camera_type c_type;
        c_type.first = cameraInfo.description().toStdString();
        c_type.second= cameraInfo.deviceName().toStdString();
        
        available_cameras.push_back(c_type);
        cout<<"c_type.first : "<<c_type.first<< " c_type.second : "<<c_type.second<<endl;
    }
    return cameras.size();
}

int VideoCall::video_stop()
{
    //kill video format and delete all allocate memory
    //No need to send stopping vc message
}

int VideoCall::video_start()
{
    //Create sender and receiver for video 
    //vc_sender = new Sender(this->target_url,this->target_port,this->active_camera);
}


void VideoCall::on_startVideo_clicked()
{
    if(true == is_connected){
        this->pkt_listen_th = new std::thread([this](){packet_listen();});
        this->in_format_ctx_set();
        this->set_encoder();
        this->set_decoder();
        this->start_sending();
        
        
    }else{
        QMessageBox msg_bx;
        msg_bx.setText("You are not connected to any client.\nSo we can not start a video call.");
        int ret= msg_bx.exec();
    }
}

void VideoCall::deactivated()
{
    if(true  == this->is_connected){
        this->is_connected = false;
        this->video_stop();
    }
}

void VideoCall::activated(QString trg_url,int trg_prt)
{
    //if(is_connected == false){
    this->target_url = trg_url.toStdString();
    this->target_port= trg_prt;
    is_connected = true;
    //}else{
    //    this->video_stop();
    //    this->video_start();
    //}
}


///SENDER DUZELTMELERI

int VideoCall::in_format_ctx_set()
{
    //starts all devices in computer
    avdevice_register_all();
    
    s_format_ctx = avformat_alloc_context();
    if (avformat_open_input(&s_format_ctx, active_camera.second.c_str(),NULL, NULL) < 0) {
        cout<<"FORMAT CONTEXT OLUSTURULAMADI"<<endl;
        return FORMAT_CONTEXT_OLUSTURULAMADI;
    }
    
    //TODO search for faster reading
    //s_format_ctx->flags = AV_CODEC_FLAG_LOW_DELAY;
    //s_format_ctx->flags = AV_CODEC_FLAG2_FAST;
    
    if (avformat_find_stream_info(s_format_ctx, NULL) < 0) {
        return STREAM_INFO_BULUNAMADI;
    }
    
    for(int i = 0;i<s_format_ctx->nb_streams;i++){
        if(s_format_ctx->streams[i]->index == AVMEDIA_TYPE_VIDEO){
            video_stream_index = i;
        }else if(s_format_ctx->streams[i]->index == AVMEDIA_TYPE_AUDIO){
            audio_stream_index = i;
        }
    }
    {
        cout<<"frame rate : "<< s_format_ctx->streams[video_stream_index]->r_frame_rate.num<<endl;
        cout<<s_format_ctx->streams[video_stream_index]->codecpar->codec_id<<endl;
    }
    
    s_codec = avcodec_find_decoder(s_format_ctx->streams[video_stream_index]->codecpar->codec_id);  
    if(NULL == s_codec){
        return AVCODEC_BULUNAMADI; 
    }
    s_codec_ctx = avcodec_alloc_context3(s_codec);
    if(s_codec_ctx == NULL){
        cout<<"s_codec_ctx is NULL"<<endl;
        return S_CODEC_CTX_OLUSTURULAMADI;
    }
    
    if(avcodec_parameters_to_context(s_codec_ctx,s_format_ctx->streams[video_stream_index]->codecpar)<0){
        cout<< "codec parameters could not be passed to S_CODEC_CTX"<<endl;
        return -1;
    }
    
    
    if(s_codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO){
        s_codec_ctx->framerate = av_guess_frame_rate(s_format_ctx,s_format_ctx->streams[video_stream_index],NULL);
        if( avcodec_open2(s_codec_ctx,s_codec,NULL)<0){
            cout<<"s_codec context acilamadi"<<endl;
            return -1;
        }
        
    }
    av_dump_format(this->s_format_ctx,video_stream_index,this->file_name.data(),0);
    return 1;
    
}

int VideoCall::output_format_ctx_set(std::string url){
    
    int ret = 0;
    AVStream *out_stream;
    AVStream *in_stream;
    out_format_ctx = NULL;
    avformat_alloc_output_context2(&out_format_ctx,NULL,"mpegts",url.c_str());
    if(out_format_ctx == NULL){
        cout<<"out_format_ctx can not be allocated"<<endl;
        return -1;
    }
    for(int i =0; i< s_format_ctx->nb_streams;i++){
        
        out_stream = avformat_new_stream(out_format_ctx,NULL);
        if(out_stream == NULL){
            cout<<"out stream olusturulamadi"<<endl;
            return -2;
        }
        in_stream = s_format_ctx->streams[i];
        if(in_stream->id == AVMEDIA_TYPE_VIDEO){
            cout<<"ENCODER OLUSTURULUYOR"<<endl;
            ret = set_encoder();
            if(ret!=0){
                return -3;
            }
            
            if (out_format_ctx->oformat->flags & AVFMT_GLOBALHEADER){
                encoder_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            }
            
            ret = avcodec_parameters_from_context(out_stream->codecpar,encoder_ctx);
            if(ret <0){
                cout<<"codec parametreleri streame gecirilemedi"<<endl;
                show_error(ret);
                return ret;
            }
            out_stream->time_base = encoder_ctx->time_base;
        }   
    }
    av_dump_format(out_format_ctx,0,url.c_str(),1);
    
    ret = avformat_write_header(out_format_ctx,NULL);
    if(ret<0){
        cout<<"format header yazilamadi"<<endl;
        show_error(ret);
        return ret;
    }
    return 0;
    
}

int VideoCall::start_sending()
{   
    
    avformat_network_init();
    int ret;
    while(false == exit_flag){
        if( is_paused == true){
            unique_lock<mutex> lck(cv_pause_mut);
            cv_pause.wait(lck);
            if(exit_flag==true){
                break;
            }
        }
        AVPacket *temp_packet;
        temp_packet = av_packet_alloc();
        av_read_frame(s_format_ctx,temp_packet);
        //cout<<"pkt size : "<<temp_packet->size<<endl;
        
        if(temp_packet->stream_index == video_stream_index){
            
            ret = avcodec_send_packet(s_codec_ctx,temp_packet);
            
            if(ret == 0){
                
                while(true){
                    AVFrame *temp_frame;
                    temp_frame = av_frame_alloc();
                    if(avcodec_receive_frame(s_codec_ctx,temp_frame) != 0){
                        av_frame_free(&temp_frame);
                        break;
                    }else{
                        //encode_and_send(temp_frame);
                        start_encoding(temp_frame);
                    }
                    //av_frame_free(&temp_frame);
                    
                }
            }
            
            av_packet_free(&temp_packet);
        }
    }
    return 1;
}

int VideoCall::start_encoding(AVFrame *frame)
{
    int ret;
    int ret2;
    
    AVFrame *t;
    t = avframeToAvframe(frame,AV_PIX_FMT_YUV420P,encoder_ctx);
    
    ret = avcodec_send_frame(encoder_ctx,t);
    if(ret<0){
        show_error(ret);
        cout<<"line 252"<<endl;
        return -4;
    }
    
    AVPacket enc_pkt;
    enc_pkt.data = NULL;
    enc_pkt.size = 0;
    av_init_packet(&enc_pkt);
    while(ret >=0){
        
        ret = avcodec_receive_packet(encoder_ctx,&enc_pkt);
        if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
            show_error(ret);
            return -5;
        }else if(ret<0){
            show_error(ret);
            cout<<"ERROR IN ENCODING LINE 259"<<endl;
            return -6;
        }
        
        prepare_and_send_data(enc_pkt);
    
    }
    return 0;
}

int VideoCall::set_encoder()
{
    int ret;
    avdevice_register_all();
    encoder = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
    if(encoder == NULL){
        cout<<"encoder can not be found"<<endl;
        return -1;
    }
    
    encoder_ctx = avcodec_alloc_context3(encoder);
    if(encoder_ctx == NULL){
        cout<<"Could not allocate video encoder context"<<endl;
        return -2;
    }
    
    encoder_ctx->bit_rate = 400000;
    encoder_ctx->width    = s_codec_ctx->width;
    encoder_ctx->height   = s_codec_ctx->coded_height;
    encoder_ctx->time_base= s_codec_ctx->time_base;
    encoder_ctx->framerate= s_codec_ctx->framerate;
    
    encoder_ctx->gop_size     = 12;
    encoder_ctx->max_b_frames = 1;
    encoder_ctx->pix_fmt      = AV_PIX_FMT_YUV420P;
    
    if(encoder->id == AV_CODEC_ID_H264){
        av_opt_set(encoder_ctx->priv_data, "preset", "slow", 0);
    }
    ret = avcodec_open2(encoder_ctx, encoder, NULL);
    if(ret <0){
        show_error(ret);
        return -3;
    }
    return 0;
}

int VideoCall::set_decoder()
{
    
    int ret = 0;
    decoder = avcodec_find_decoder(AV_CODEC_ID_MPEG4);
    
    if(decoder == NULL){
        cout<<"decoder olusturulamadi "<<endl;
        return -1;
    }
    
    decoder_ctx = avcodec_alloc_context3(decoder);
    if(decoder_ctx ==NULL){
        cout<<"decoder allocation hatasi"<<endl;
        return -2;
    }
    
    decoder_ctx->bit_rate = 400000;
    decoder_ctx->width    = encoder_ctx->width;
    decoder_ctx->height   = encoder_ctx->height;
    decoder_ctx->time_base= AVRational{1,30};
    decoder_ctx->framerate= AVRational{30,1};
    
    
    decoder_ctx->gop_size     = 10;
    decoder_ctx->max_b_frames = 1;
    decoder_ctx->pix_fmt      = AV_PIX_FMT_YUV420P;
    
    ret = avcodec_open2(decoder_ctx,decoder,NULL);
    if(ret <0){
        show_error(ret);
        cout<<"decoder acma hatasi"<<endl;
        return -3;
    }
    
}

int VideoCall::decode_and_show(AVPacket enc_pkt)
{
    int ret2 = 0;
    ret2 = avcodec_send_packet(decoder_ctx,&enc_pkt);
    if(ret2== AVERROR(EAGAIN)||ret2 == AVERROR_EOF){
        show_error(ret2);
        return -7;
    }else if(ret2<0){
        show_error(ret2);
        cout<<"ERROR IN DECODING ENCODED PACKET"<<endl;
        return -8;
    }
    if(ret2>=0){
        
        AVFrame *decoded_frame;
        decoded_frame = av_frame_alloc();
        
        while(true){
            if(avcodec_receive_frame(decoder_ctx,decoded_frame) != 0){
                av_frame_free(&decoded_frame);
                break;
            }else{
                
                cv::Mat temp_m;
                avframeToMat(decoded_frame,temp_m);
                cv::imshow("decoded frame",temp_m);
                cv::waitKey(1);
            }
        }
        av_frame_free(&decoded_frame);
        
    }
    return 0;
    
}

//size = 20004
struct data_paket{
        int     packet_size;
        uint8_t data[30000];
};

int VideoCall::prepare_and_send_data(AVPacket &pkt)
{
    data_paket raw_data;
    raw_data.packet_size = pkt.size;
    //uint8_t raw_data[pkt.size];
    memcpy(raw_data.data,pkt.data,pkt.size);
    //AVPacket temp;
    //av_packet_from_data(&temp,raw_data,pkt.size);
    //decode_and_show(temp);
    
    int size = sizeof(raw_data)/sizeof(uint8_t);
    cout<<"raw size : "<<size<<endl;
    int sockfd;
    struct sockaddr_in servaddr;
    
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        std::cout << "UDP socket creating error(sending message)." << std::endl;
    }
    
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,0,sizeof(int));
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(23000);
    servaddr.sin_addr.s_addr = inet_addr((const char*)"192.168.1.7");
    
    //if(inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr)<=0){
    //    std::cout << "Can not inet_pton." << std::endl;
    //}
    uint8_t *raw_data_pkt;
    cout<<"raw data pkt size: "<<raw_data.packet_size<<endl;
    raw_data_pkt = reinterpret_cast<uint8_t*>(&raw_data);
    sendto(sockfd, raw_data_pkt,30004, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    ::close(sockfd);    
}

void VideoCall::packet_listen() {
    struct sockaddr_in server_addr, client_addr;
    int ret;
    int server_sockfd;
    server_sockfd = socket(AF_INET,SOCK_DGRAM,0);
    int yes = 1;
    if(server_sockfd <0)
    {
        std::cout << "UDP socket creating error." << std::endl;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr((const char*)"192.168.1.7");
    server_addr.sin_port = htons(target_port);
    
    if( 0 > bind(server_sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)))
    {
        std::cout << "UDP socket binding error." << std::endl;
    }
    int received_size;
    socklen_t len = sizeof(client_addr);
    
    while (true) {
        // recv(server_sockfd, tempdata, 30, 0);
        uint8_t tempdata[30004];
        received_size = recvfrom(server_sockfd, tempdata, 30004, 0, (struct sockaddr *)&client_addr, &len);
        char client_ip[16];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, 16);
        
        
        //data_paket *packet;
        //packet = reinterpret_cast<data_paket*>(tempdata);
        //uint8_t tdata[packet->packet_size];
        //memcpy(tdata,packet->data,packet->packet_size);
        //AVPacket temp;
        //av_packet_from_data(&temp,tdata,packet->packet_size);
        //cout<<"packet size in listen : "<<packet->packet_size<<endl;
        ////cout<<"AAA"<<endl;
        ////decode_and_show(temp);
        //
        //int ret2 = 0;
        //ret2 = avcodec_send_packet(decoder_ctx,&temp);
        //if(ret2== AVERROR(EAGAIN)||ret2 == AVERROR_EOF){
        //    show_error(ret2);
        //    continue;
        //}else if(ret2<0){
        //    show_error(ret2);
        //    cout<<"ERROR IN DECODING ENCODED PACKET"<<endl;
        //    continue;
        //}
        //if(ret2>=0){
        //    
        //    AVFrame *decoded_frame;
        //    decoded_frame = av_frame_alloc();
        //    
        //    while(true){
        //        if(avcodec_receive_frame(decoder_ctx,decoded_frame) != 0){
        //            av_frame_free(&decoded_frame);
        //            break;
        //        }else{
        //            
        //            cv::Mat temp_m;
        //            avframeToMat(decoded_frame,temp_m);
        //            //cv::cvtColor(temp_m, temp_m, cv::COLOR_BGR2RGB);
        //            //this->ui->image_lbl->setPixmap(QPixmap::fromImage(QImage(temp_m.data, temp_m.cols, temp_m.rows, temp_m.step, QImage::Format_RGB888)));
        //            
        //            cv::imshow("decoded frame",temp_m);
        //            //cv::waitKey(1);
        //            av_frame_free(&decoded_frame);
        //            break;
        //        }
        //    }
        //    
        //}
        //
        //
        //packet = nullptr;
    }
}
