#ifndef VIDEOCALL_H
#define VIDEOCALL_H

#include <QWidget>
#include <iostream>
#include "VideoCall/receiver.h"
#include "VideoCall/sender.h"
#include <QCameraInfo>
#include <QMessageBox>

using namespace  std;
using namespace vc;
namespace Ui {
    class VideoCall;
}

class VideoCall : public QWidget
{
        Q_OBJECT
        
    public:
        explicit VideoCall(QWidget *parent = nullptr,string tar_url="",int target_prt=23000,Udp_Socket *mw_udp_sock=nullptr);
        ~VideoCall();
        int detect_cameras();
        int video_stop();
        int video_start();
        Udp_Socket *vc_udp_sock;
    private:
        
        Ui::VideoCall *ui;
        string target_url;
        int target_port;
        Receiver *vc_receiver;
        Sender   *vc_sender;
        camera_type active_camera;
        vector<camera_type> available_cameras;
        bool is_connected = false;
        
    signals:
        void vc_stop();
        void vc_start();
        
        
    public slots:
        void on_startVideo_clicked();
        void deactivated();
        void activated(QString trg_url,int trg_port);
};

#endif // VIDEOCALL_H
