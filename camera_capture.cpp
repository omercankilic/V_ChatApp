#include "camera_capture.h"
#include <iostream>
#include <Helper_Functions.h>
//#include "VideoCall/sender.h"
using namespace std;

Camera_Capture::Camera_Capture()
{
    cout<<"Videocapture class started" <<endl;
    
}

void Camera_Capture::set_format_ctx()
{
    int video_index;
    fmt_ctx = avformat_alloc_context();
    if(fmt_ctx == nullptr){
        cout<< "format context olusturulamadi";
        return;
    }
    avdevice_register_all();
    
    fmt_ctx->flags = AV_CODEC_FLAG_LOW_DELAY;
    //fmt_ctx->flags = AV_CODEC_FLAG2_FAST;
    this->input_format = av_find_input_format("video4linux2");
    if(input_format == nullptr){
        cout<<"Input format bulunamadi"<<endl;
        return;
    }
    //Dictionary options can be set here
    // {
    //         if( av_dict_set(&(this->dict_ctx),"framerate","25",0)<0){
    //                 cout<<"dictionary set frame rate error "<<endl;
    //                 return;
    //         }
    // }
    
    //
    
    if( avformat_open_input(&(this->fmt_ctx),this->file_name,nullptr,nullptr) < 0 ){
        cout<<"can not open format context "<<endl;
    }
    
    
    // lookup infor
    if(avformat_find_stream_info(this->fmt_ctx,NULL)<0)
    { return;}
    
    
    for(int i = 0;i<this->fmt_ctx->nb_streams;i++){
        cout<<"i : "<<i<<endl;
        if(fmt_ctx->streams[i]->index == AVMEDIA_TYPE_VIDEO){
            video_index = i;
            cout<<"video index : "<<video_index<<endl;
            break;
        }
    }
    
    codec = avcodec_find_decoder(fmt_ctx->streams[video_index]->codecpar->codec_id);  
    if(NULL == codec){
        cout<<"codec bulunamadi"; 
    }
    ctx = avcodec_alloc_context3(codec);
    //
    if(ctx == NULL){
        cout<<"s_codec_ctx is NULL"<<endl;
    }
    
    if(avcodec_parameters_to_context(ctx,fmt_ctx->streams[video_index]->codecpar)<0){
        cout<< "codec parameters could not be passed to S_CODEC_CTX"<<endl;
        
    }
    
    
    if(ctx->codec_type == AVMEDIA_TYPE_VIDEO){
        ctx->framerate = av_guess_frame_rate(fmt_ctx,fmt_ctx->streams[0],NULL);
        if( avcodec_open2(ctx,codec,NULL)<0){
            cout<<"s_codec context acilamadi"<<endl;
            
        }
        
    }
    
    //prints device information
    av_dump_format(this->fmt_ctx,0,this->file_name,0);
    
    AVPacket *temp_packet =av_packet_alloc();
  
    while(true){
        
        // cout<< "1 : "<<setprecision(std::numeric_limits<long double>::digits10 + 1)<<get_current_time_millisecond()<<endl;
        av_read_frame(this->fmt_ctx,temp_packet);
        cout<<"packet->stream_index : "<<temp_packet->stream_index<<endl;
        
        if(temp_packet->stream_index == video_index){
            int ret = avcodec_send_packet(ctx,temp_packet);
            if(ret<0){
                if(ret == AVERROR(EAGAIN)){
                    cout<<"EAGAIN"<<endl;
                }
                cout<<ret<<endl;
                char hata[1000];
                int a =av_strerror(ret,hata,1000);
                cout<<hata<<endl;
            }
            if(ret==0){
                cout<<endl<<"BASARILI"<<endl;
                while(ret==0){
                    AVFrame *frame;
                    AVFrame *dst_frame;
                    dst_frame = av_frame_alloc();
                    frame = av_frame_alloc();
                    
                    ret =avcodec_receive_frame(ctx,frame);
                    if(ret != 0){
                        cout<<"ret =! 0"<<endl;
                        continue;
                    }
                    cout<<"frame show"<<endl;
                    
                    cv::Mat image;
                    int width = frame->width;
                    int height = frame->height;
                    
                    // Allocate the opencv mat and store its stride in a 1-element array
                    if (image.rows != height || image.cols != width || image.type() != CV_8UC3) image = cv::Mat(height, width, CV_8UC3);
                    int cvLinesizes[1];
                    cvLinesizes[0] = image.step1();
                    
                    // Convert the colour format and write directly to the opencv matrix
                    SwsContext* conversion = sws_getContext(width, height, (AVPixelFormat) frame->format, width, height, AVPixelFormat::AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
                    sws_scale(conversion, frame->data, frame->linesize, 0, height, &image.data, cvLinesizes);
                    sws_freeContext(conversion);
                    
                    cv::imshow("title",image);
                    cv::waitKey(1);
                    av_frame_free(&frame);
                    av_frame_free(&dst_frame);
                }
            }
        }

    }
    
    
    
    
    
}

void Camera_Capture::set_dict_opt()
{
    
}

int Camera_Capture::start_video_from_cam()
{
    Mat frame;
    //--- INITIALIZE VIDEOCAPTURE
    VideoCapture cap;
    // open the default camera using default API
    // cap.open(0);
    // OR advance usage: select any API backend
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    // open selected camera using selected API
    cap.open(deviceID + apiID);
    // check if we succeeded
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    //--- GRAB AND WRITE LOOP
    cout << "Start grabbing" << endl
         << "Press any key to terminate" << endl;
    for (;;)
    {
        // wait for a new frame from camera and store it into 'frame'
        
        
        cout<< "1 : "<<setprecision(std::numeric_limits<long double>::digits10 + 1)<<get_current_time_millisecond()<<endl;
        cap.read(frame);
        
        // encoder acma
        // encode 
        // avformat_w
        
        
        cout<<"2 : "<< setprecision(std::numeric_limits<long double>::digits10 + 1)<<get_current_time_millisecond()<<endl;
        
        
        // check if we succeeded
        if (frame.empty()) {
            cout<<"empty frame"<<endl;
            // cerr << "ERROR! blank frame grabbed\n";
            // break;
        }
        // show live and wait for a key with timeout long enough to show images
        imshow("Live", frame);
        waitKey(5);
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
    
    
}
