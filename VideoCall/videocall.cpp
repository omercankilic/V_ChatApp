#include "videocall.h"
#include "ui_videocall.h"

VideoCall::VideoCall(QWidget *parent,string tar_url,int target_prt,Udp_Socket *mw_udp_sock) :
    QWidget(parent),
    ui(new Ui::VideoCall),target_url(tar_url),target_port(target_prt)
{
    this->vc_udp_sock = mw_udp_sock;
    detect_cameras();
    ui->setupUi(this);
    //TODO new ui will be shown here and asked to select camera
}

VideoCall::~VideoCall()
{
    delete ui;
}

int VideoCall::detect_cameras()
{
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : cameras){
        camera_type c_type;
        c_type.first = cameraInfo.description().toStdString();
        c_type.second= cameraInfo.deviceName().toStdString();
        available_cameras.push_back(c_type);
        cout<<"c_type.first : "<<c_type.first<< " c_type.second : "<<c_type.second<<endl;
    }
    return cameras.size();
}

int VideoCall::video_stop()
{
    //kill video format and delete all allocate memory
    //No need to send stopping vc message
}

int VideoCall::video_start()
{
    //Create sender and receiver for video 
}


void VideoCall::on_startVideo_clicked()
{
    if(false != is_connected){
        
         
         //ui->ipList->setEditable(true);
         //ui->ipList->lineEdit()->setReadOnly(true);
         //ui->ipList->lineEdit()->setAlignment(Qt::AlignLeft);
         //while (!file.atEnd()) {
         //   QByteArray line = file.readLine();
         //   ui->ipList->addItem(line);
         //}
    }else{
        QMessageBox msg_bx;
        msg_bx.setText("You are not connected to any client.\nSo we can not start a video call.");
        int ret= msg_bx.exec();
    }
}

void VideoCall::deactivated()
{
    if(true  == this->is_connected){
        this->is_connected = false;
        this->video_stop();
    }
}

void VideoCall::activated(QString trg_url,int trg_prt)
{
    //if(is_connected == false){
        this->target_url = trg_url.toStdString();
        this->target_port= trg_prt;
        is_connected = true;
    //}else{
    //    this->video_stop();
    //    this->video_start();
    //}
}
