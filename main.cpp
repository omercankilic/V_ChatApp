#include <openwindow.h>
#include <QApplication>
#include <VideoCall/videocall.h>
#include "camera_capture.h"
#include "VideoCall/sender.h"
#include "VideoCall/videocall.h"
using namespace vc;


int main(int argc, char *argv[])
{
        QApplication a(argc, argv);
       
        OpenWindow open_window;
        open_window.show();
        return a.exec();
        //VideoCall v;
        //v.detect_cameras();
        //Camera_Capture c;
        //c.set_format_ctx();
        //VideoCall *v;
        //QWidget q;
        //v = new VideoCall(&q,"192.168.1.6","52000");
        //v->detect_cameras();
        //Sender *s;
        //s= new Sender();
        //s->in_format_ctx_set();
        // //s->out_format_ctx_set();
        //s->set_encoder();
        //s->set_decoder();
        //s->start_sending();
        
        return 0;
}
