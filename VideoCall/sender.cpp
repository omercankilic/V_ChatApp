#include "sender.h"


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
    if (avformat_open_input(&s_format_ctx, "/dev/video0",NULL, NULL) < 0) {
        cout<<"FORMAT CONTEXT OLUSTURULAMADI"<<endl;
        return FORMAT_CONTEXT_OLUSTURULAMADI;
    }
    
    //TODO search for faster reading
    s_format_ctx->flags = AV_CODEC_FLAG_LOW_DELAY;
    s_format_ctx->flags = AV_CODEC_FLAG2_FAST;
    
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
    return 1;
    
}

int vc::Sender::out_format_ctx_set(){
    
    string out_file_name = "udp://" +target_url+ ":"+target_port;
    if(avformat_alloc_output_context2(&out_format_ctx,NULL,NULL,out_file_name.data())<0){
        cout<<"Output format context could not be created"<<endl;
        return -1;
    }
    
    AVStream *out_stream;
    
    out_stream = avformat_new_stream(out_format_ctx,NULL);
    if(!out_stream){
        cout<<"Output stream could not be created"<<endl;
        return -1;
    }
    
    out_codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if(!out_codec){
        cout<<"out encoder could not be found"<<endl;
        return -1;
    }
    out_codec_ctx = avcodec_alloc_context3(out_codec);
    
    out_codec_ctx->max_b_frames = 0;
    out_codec_ctx->width = s_codec_ctx->width;
    out_codec_ctx->height = s_codec_ctx->height;
    out_codec_ctx->sample_aspect_ratio = s_codec_ctx->sample_aspect_ratio;
    out_codec_ctx->pix_fmt = s_codec_ctx->pix_fmt;
    out_codec_ctx->time_base = s_codec_ctx->time_base;
    out_codec_ctx->framerate = s_codec_ctx->framerate;
    
    if(out_format_ctx->oformat->flags & AVFMT_GLOBALHEADER){
        out_codec_ctx->flags |=AV_CODEC_FLAG_GLOBAL_HEADER;
        
    }
    
    if(avcodec_open2(out_codec_ctx,out_codec,NULL)){
        cout<<"out_codec_ctx could not be created "<<endl;
        return -1;
    }
    av_dump_format(out_format_ctx,0,out_file_name.data(),1);
    if (!(out_format_ctx->oformat->flags & AVFMT_NOFILE)) {
         int ret = avio_open(&out_format_ctx->pb, out_file_name.data(), AVIO_FLAG_WRITE);
      
         if(ret<0){
             cout<<"output stream could not be started"<<endl;
             return -1;
         }
    }
    return 1;
}

int vc::Sender::encode_and_send(AVFrame *in_frame)
{
    int ret = avcodec_send_frame(out_codec_ctx,in_frame);
    if(ret <0){
        cout<<"frame i encodera gonderme hatasi"<<endl;
        return -1;
    }
    AVPacket *av_pkt;
    av_pkt = av_packet_alloc();
    while(ret>0){
        ret =avcodec_receive_packet(out_codec_ctx,av_pkt);
        
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
            cout<<"AVERROR EAGAIN OR EOF HATASI"<<endl;       
            return -1;
        }
        av_pkt->stream_index = video_stream_index;
        av_packet_rescale_ts(av_pkt,out_codec_ctx->time_base,out_format_ctx->streams[video_stream_index]->time_base);
        ret = av_interleaved_write_frame(out_format_ctx,av_pkt);    
    }
    
}

int vc::Sender::start_sending()
{
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
        
        avcodec_send_packet(s_codec_ctx,temp_packet);
        
        AVFrame *temp_frame;
        while(avcodec_receive_frame(s_codec_ctx,temp_frame)){
            encode_and_send(temp_frame);
        }
        av_packet_free(&temp_packet);
        av_frame_free(&temp_frame);
    }
    return 1;
}
