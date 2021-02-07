#include "FFmpeg_Headers.h"
#include <chrono>
#include <limits>
#include <iomanip>
#include <cmath>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include "video_funcs.h"
using namespace cv;
namespace vc{
    void avframeToMat(const AVFrame *frame, Mat& image)
    {
        
        int width = frame->width;
        int height = frame->height;
        if (image.rows != height || image.cols != width || image.type() != CV_8UC3) image = cv::Mat(height, width, CV_8UC3);
        int cvLinesizes[1];
        cvLinesizes[0] = image.step1();
        
        // Convert the colour format and write directly to the opencv matrix
        SwsContext* conversion = sws_getContext(width, height, (AVPixelFormat) frame->format, width, height, AVPixelFormat::AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
        sws_scale(conversion, frame->data, frame->linesize, 0, height, &image.data, cvLinesizes);
        sws_freeContext(conversion);
        
    }
    AVFrame* avframeToAvframe(const AVFrame *in_frame,AVPixelFormat target_pix_fmt,AVCodecContext *enc_ctx){
        int ret;
        AVFrame *target_frame = av_frame_alloc();
        assert(target_frame);
        target_frame->format = enc_ctx->pix_fmt;
        target_frame->width  = enc_ctx->width;
        target_frame->height = enc_ctx->height;
        
        
        int number_of_bytes=av_image_get_buffer_size(enc_ctx->pix_fmt,enc_ctx->width,enc_ctx->height,1); 
        assert(number_of_bytes);
        uint8_t *dataBuffer = new uint8_t[number_of_bytes*sizeof(uint8_t)];// (uint8_t*) av_malloc (number_of_bytes*sizeof(uint8_t));
        
        //target_frame->data[0]=dataBuffer;
        
        // avpicture_fill((AVPicture *)frameYUV, dataBuffer, AV_PIX_FMT_YUV420P, c->width, c->height);  
        av_image_fill_arrays(target_frame->data,
                             target_frame->linesize,
                             dataBuffer,
                             (AVPixelFormat)target_frame->format,
                             target_frame->width,
                             target_frame->height,1);
        
        SwsContext *scaler_ctx = sws_getContext(
                    enc_ctx->width,
                    enc_ctx->height,
                    (AVPixelFormat)in_frame->format,
                    enc_ctx->width,
                    enc_ctx->height,
                    (AVPixelFormat)enc_ctx->pix_fmt,
                    SWS_BILINEAR,
                    0,
                    0,
                    0
                    );
        ret = sws_scale(scaler_ctx,(const uint8_t *const *)in_frame->data,in_frame->linesize,0,enc_ctx->height,target_frame->data,target_frame->linesize);
        sws_freeContext(scaler_ctx);
        scaler_ctx = nullptr;
        assert(ret);
        //free(dataBuffer);
        return target_frame;
    }
    
    int show_error(int error_key){
        char er[200];
        int ret =  av_strerror(error_key,er,200);
        if(ret == 0){
            cout<<er<<endl;
            return 0;
        }else{
            cout<<"Error type could not be found."<<endl;
            return ret;
        }
    }
    
    long double get_current_time_millisecond()
    {
        
        long double millisec_since_epoch = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
        return millisec_since_epoch;
        
    }
    
}
