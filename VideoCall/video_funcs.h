#ifndef VIDEO_FUNCS_H
#define VIDEO_FUNCS_H
#include "FFmpeg_Headers.h"
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/highgui.hpp>
namespace vc{
    void avframeToMat(const AVFrame * frame, cv::Mat& image);
}

#endif // VIDEO_FUNCS_H
