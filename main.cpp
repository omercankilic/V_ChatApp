#include <openwindow.h>
#include <QApplication>
#include <VideoCall/videocall.h>
#include "camera_capture.h"



int main(int argc, char *argv[])
{
        QApplication a(argc, argv);
       
        //OpenWindow open_window;
        //open_window.show();
        //return a.exec();
        //VideoCall v;
        //v.detect_cameras();
        Camera_Capture c;
        c.set_format_ctx();
        
        return 0;
}
