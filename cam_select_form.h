#ifndef CAM_SELECT_FORM_H
#define CAM_SELECT_FORM_H

#include <QDialog>
#include "ui_cam_select_form.h"
#include <QCameraInfo>
#include <vector>
#include "FFmpeg_Headers.h"
using namespace std;
namespace Ui {
    class cam_select_form;
}

class cam_select_form : public QDialog
{
        Q_OBJECT
        
    public:
        explicit cam_select_form(QWidget *parent = nullptr);
        ~cam_select_form();
        vector<camera_type> available_cameras;
        int detect_cameras()
        {
            QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
            for (const QCameraInfo &cameraInfo : cameras){
                camera_type c_type;
                c_type.first = cameraInfo.description().toStdString();
                c_type.second= cameraInfo.deviceName().toStdString();
                
                available_cameras.push_back(c_type);
                ui->cam_list->addItem(QString::fromStdString(c_type.first));
            } 
            
            return cameras.size();
            
        }
    public slots:
        QString on_cam_select_button_clicked();
        
    private:
        Ui::cam_select_form *ui;
};

#endif // CAM_SELECT_FORM_H
