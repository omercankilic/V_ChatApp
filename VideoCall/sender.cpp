#include "sender.h"
#include "video_funcs.h"
vc::Sender::Sender()
{
}

vc::Sender::Sender(string target_url, string target_port,camera_type active_c):target_url(target_url),target_port(target_port),active_cam(active_c)
{
    
}

int vc::Sender::in_format_ctx_set()
{
    //starts all devices in computer
    avdevice_register_all();
    
    s_format_ctx = avformat_alloc_context();
    if (avformat_open_input(&s_format_ctx, "/dev/video2",NULL, NULL) < 0) {
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

int vc::Sender::out_format_ctx_set(){
    int ret;
    const char *o_file_name = "udp://127.0.0.1:12345";
    
    ret =avformat_alloc_output_context2(&out_format_ctx,NULL,"mpeg2video",o_file_name);
    if(ret < 0){
        cout<<"Output format context could not be created"<<endl;
        show_error(ret);
        return -1;
    }
    
    AVStream *out_stream;
    
    out_stream = avformat_new_stream(out_format_ctx,NULL);
    if(!out_stream){
        cout<<"Output stream could not be created"<<endl;
        return -1;
    }
    
    out_codec = avcodec_find_encoder_by_name("mpeg2video");
    if(NULL == out_codec){
        cout<<"out encoder could not be found"<<endl;
        return -1;
    }
    out_codec_ctx = avcodec_alloc_context3(out_codec);
    if(out_codec_ctx == NULL){
        cout<<"Could not allocate video encoder context"<<endl;
        return -2;
    }
    out_codec_ctx->max_b_frames = 0;
    out_codec_ctx->width  = s_codec_ctx->width;
    out_codec_ctx->height = s_codec_ctx->height;
    
    out_codec_ctx->bit_rate = 4000000;
    out_codec_ctx->max_b_frames = 0;
    out_codec_ctx->gop_size = 10;
    out_codec_ctx->time_base = AVRational{1,30};
    out_codec_ctx->framerate = AVRational{30,1};
    out_codec_ctx->pix_fmt   =AV_PIX_FMT_YUV420P;
    
    ret = avcodec_open2(out_codec_ctx,out_codec,NULL);
    if(ret){
        cout<<"out_codec_ctx could not be created "<<endl;
        show_error(ret);
        return -1;
    }
    
    ret =avcodec_parameters_from_context(out_format_ctx->streams[video_stream_index]->codecpar,out_codec_ctx);
    if(ret<0){
        cout<<"failed to copy encoder parameters to output stream"<<endl;
    }
    
    if(out_format_ctx->oformat->flags & AVFMT_GLOBALHEADER){
        out_codec_ctx->flags |=AV_CODEC_FLAG_GLOBAL_HEADER;
        
    }

    
    av_dump_format(out_format_ctx,video_stream_index,o_file_name,1);
    if (!(out_format_ctx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&out_format_ctx->pb, o_file_name, 0);
        
        if(ret<0){
            cout<<"output stream could not be started"<<endl;
            return -1;
        }
    }
    ret = avformat_write_header(out_format_ctx,NULL);
    if(ret<0){
        return show_error(ret);
    }
    
    return 1;
}
static int a = 1;
int vc::Sender::encode_and_send(AVFrame *frame)
{
    int ret;
    //AVFrame *p_frame;
    //p_frame = av_frame_alloc();
    //int width = frame->width;
    //int height= frame->height;
    //SwsContext* conversion = sws_getContext(width, height, (AVPixelFormat) frame->format, 704, 576, AVPixelFormat::AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    //avcodec_align_dimensions2(out_codec_ctx,&frame->width,&frame->height,p_frame->linesize);
    //
    //sws_scale(conversion, frame->data, frame->linesize, 0, height,(p_frame->data), p_frame->linesize);
    
    
    ret = avcodec_send_frame(out_codec_ctx,frame);
    if(ret <0){
        cout<<"frame i encoder gonderme hatasi"<<endl;
        show_error(ret);
        return -1;
    }
    av_frame_free(&frame);
    cout<<"encodera gitti"<<endl;
    AVPacket *av_pkt;
    av_pkt = av_packet_alloc();
    while(ret==0){
        ret =avcodec_receive_packet(out_codec_ctx,av_pkt);
        cout<<"a: " <<a<<endl;;
        a++;
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
            cout<<"AVERROR EAGAIN OR EOF HATASI"<<endl;   
            show_error(ret);
            break;
        }
        
        //av_packet_rescale_ts(av_pkt, out_codec_ctx->time_base, out_format_ctx->streams[video_stream_index]->time_base);
        
        av_pkt->stream_index = video_stream_index;
        
        ret = av_write_frame(out_format_ctx,av_pkt);
        if(ret<0){
            show_error(ret);
        }
        cout<<"av_pkt pts and dts : "<<(av_pkt->pts)/1000<<" "<<(av_pkt->dts)/1000<<endl;
    
        
    }
    av_packet_free(&av_pkt);
    
}

int vc::Sender::start_sending()
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
                        //cv::Mat temp_m;
                        //cout<<"frame size : "<<temp_frame->pkt_size<<endl;
                        //avframeToMat(temp_frame,temp_m);
                        //////cout<<temp_m.
                        //cv::imshow("title",temp_m);
                        //cv::waitKey(1);
                        //encode_and_send(temp_frame);
                        AVPacket *t_pkt;
                        t_pkt = av_packet_alloc();
                        start_encoding(temp_frame,t_pkt);
                        //av_packet_free(&t_pkt);
                    }
                    //av_frame_free(&temp_frame);
                    
                }
            }
            
            av_packet_free(&temp_packet);
        }
    }
    return 1;
}

int vc::Sender::start_encoding(AVFrame *frame,AVPacket *pkt)
{
    int ret;int ret2;
    
    AVFrame *t;
    t = avframeToAvframe(frame,AV_PIX_FMT_YUV420P,encoder_ctx);
    
    ret = avcodec_send_frame(encoder_ctx,t);
    if(ret<0){
        show_error(ret);
        cout<<"line 252"<<endl;
        return -4;
    }
    while(ret >=0){
        
        ret = avcodec_receive_packet(encoder_ctx,pkt);
        if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
            show_error(ret);
            return -5;
        }else if(ret<0){
            show_error(ret);
            cout<<"ERROR IN ENCODING LINE 259"<<endl;
            return -6;
        }
        //cout<< "BASARILI ENCODE ISLEMI at "<<setprecision(10)<<get_current_time_millisecond()<<endl;
        cout<<"encoded packet size : "<<pkt->size<< endl;
        
        ret2 = avcodec_send_packet(decoder_ctx,pkt);
        if(ret2== AVERROR(EAGAIN)||ret2 == AVERROR_EOF){
            show_error(ret2);
            return -7;
        }else if(ret<0){
            show_error(ret2);
            cout<<"ERROR IN DECODING ENCODED PACKET"<<endl;
            return -8;
        }
        if(ret2==0){
            AVFrame *decoded_frame;
            decoded_frame = av_frame_alloc();
            
            while(true){
                if(avcodec_receive_frame(decoder_ctx,decoded_frame) != 0){
                    av_frame_free(&decoded_frame);
                    break;
                }else{
                 
                    cv::Mat temp_m;
                    cout<<"frame size : "<<decoded_frame->pkt_size<<endl;
                    avframeToMat(decoded_frame,temp_m);
                    cv::imshow("decoded frame",temp_m);
                    cv::waitKey(1);
                }
            }
        
        }
        
    }
    
}

int vc::Sender::set_encoder()
{
    int ret;
    avdevice_register_all();
    encoder = avcodec_find_encoder(AV_CODEC_ID_MPEG4);// _by_name("mpeg1video");
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
    encoder_ctx->time_base= AVRational{1, 30};
    encoder_ctx->framerate= AVRational{30, 1};
    
    encoder_ctx->gop_size     = 10;
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
}

int vc::Sender::set_decoder()
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
    
    // decoder parameters setting
    //encoder_ctx->bit_rate = 400000;
    //encoder_ctx->width    = s_codec_ctx->width;
    //encoder_ctx->height   = s_codec_ctx->coded_height;
    //encoder_ctx->time_base= AVRational{1, 30};
    //encoder_ctx->framerate= AVRational{30, 1};
    //
    //encoder_ctx->gop_size     = 10;
    //encoder_ctx->max_b_frames = 1;
    //encoder_ctx->pix_fmt      = AV_PIX_FMT_YUV420P;
    
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
