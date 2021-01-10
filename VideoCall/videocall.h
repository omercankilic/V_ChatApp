#ifndef VIDEOCALL_H
#define VIDEOCALL_H

#include <QWidget>
#include <iostream>
#include "VideoCall/receiver.h"
#include "VideoCall/sender.h"
#include <QCameraInfo>

using namespace  std;
using namespace vc;
namespace Ui {
    class VideoCall;
}

class VideoCall : public QWidget
{
        Q_OBJECT
        
    public:
        explicit VideoCall(QWidget *parent = nullptr,string tar_url="",string target_prt="");
        ~VideoCall();
        int detect_cameras();
        
    private:
        
        Ui::VideoCall *ui;
        string target_url;
        string target_port;
        Receiver *vc_receiver;
        Sender   *vc_sender;
        vector<camera_type> available_cameras;
        string active_camera;
        
    signals:
        void vc_stop();
        
        
};

#endif // VIDEOCALL_H
