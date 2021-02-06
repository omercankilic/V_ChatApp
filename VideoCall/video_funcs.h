#ifndef VIDEO_FUNCS_H
#define VIDEO_FUNCS_H
#include "FFmpeg_Headers.h"
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <iomanip>
namespace vc{
    void avframeToMat(const AVFrame * frame, cv::Mat& image);
    int show_error(const int error_key);
    long double get_current_time_millisecond();
    AVFrame* avframeToAvframe(const AVFrame *in_frame,AVPixelFormat target_pix_fmt,AVCodecContext *enc_dec);
}

#endif // VIDEO_FUNCS_H
