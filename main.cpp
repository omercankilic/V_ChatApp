#include <openwindow.h>
#include <QApplication>

int main(int argc, char *argv[])
{
        QApplication a(argc, argv);
       
        OpenWindow open_window;
        open_window.show();
        //MainWindow w;
        //w.show();
        return a.exec();
        //Camera_Capture temp_capture;
        //temp_capture.start_video_from_cam();
        //temp_capture.set_format_ctx();
        
        return 0;
}
