#ifndef FFMPEG_HEADERS_H
#define FFMPEG_HEADERS_H
#include <iostream>
#include <condition_variable>
#include <mutex>
using namespace  std;
typedef std::pair<string,string> camera_type;

extern "C"{
    #include    <libavcodec/avcodec.h>
    #include    <libavformat/avformat.h>
    #include    <libavutil/avconfig.h>
    #include    <libavdevice/avdevice.h>
    #include    <libavfilter/avfilter.h>
    #include    <libswscale/swscale.h>
    #include    <libswresample/swresample.h>
    #include    <libavutil/imgutils.h>  
}


#endif // FFMPEG_HEADERS_H
