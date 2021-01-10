#include "FFmpeg_Headers.h"

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/highgui.hpp>
using namespace cv;
namespace vc{
    void avframeToMat(const AVFrame * frame, Mat& image)
    {
        //int width = frame->width;
        //int height = frame->height;
        //
        //// Allocate the opencv mat and store its stride in a 1-element array
        //if (image.rows != height || image.cols != width || image.type() != CV_8UC3) image = Mat(height, width, CV_8UC3);
        //int cvLinesizes[1];
        //cvLinesizes[0] = image.step1();
        //
        //// Convert the colour format and write directly to the opencv matrix
        //SwsContext* conversion = sws_getContext(width, height, (AVPixelFormat) frame->format, width, height, AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
        //sws_scale(conversion, frame->data, frame->linesize, 0, height, &image.data, cvLinesizes);
        //sws_freeContext(conversion);
        
    
    }
}
