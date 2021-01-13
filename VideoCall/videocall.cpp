#include "videocall.h"
#include "ui_videocall.h"

VideoCall::VideoCall(QWidget *parent,string tar_url,string target_prt) :
    QWidget(parent),
    ui(new Ui::VideoCall),target_url(tar_url),target_port(target_prt)
{
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

