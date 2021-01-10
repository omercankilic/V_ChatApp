#ifndef CAMERA_CAPTURE_H
#define CAMERA_CAPTURE_H
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <FFmpeg_Headers.h>
#include "VideoCall/video_funcs.h"
using namespace std;
using namespace cv;


class Camera_Capture
{
        public:
                Camera_Capture();
                void set_format_ctx();
                void set_dict_opt();
                int start_video_from_cam();
        private:
                AVFormatContext *fmt_ctx        = nullptr;
                AVCodec *codec = nullptr;
                AVCodecContext *ctx = nullptr;
                AVDictionary *dict_ctx          = nullptr;
                AVInputFormat *input_format   = nullptr;
                const char *file_name = "/dev/video2";
                
               
                int height_size;
                int width_size;
                const char *format = "video4linux2";
};

#endif // CAMERA_CAPTURE_H
