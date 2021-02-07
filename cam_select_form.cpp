#include "cam_select_form.h"
#include "ui_cam_select_form.h"

cam_select_form::cam_select_form(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cam_select_form)
{
    ui->setupUi(this);
}

cam_select_form::~cam_select_form()
{
    delete ui;
}

QString cam_select_form::on_cam_select_button_clicked()
{
    if(available_cameras.empty()){
        return "NOCAMCONNECTED";
    }else{
        
        for(int i = 0;i<available_cameras.size();i++){
            if( ui->cam_list->currentText().toStdString()== available_cameras.at(i).first){
                return QString::fromStdString(available_cameras.at(i).second); 
            } 
        } 
    }
    
    return "NOCAMCONNECTED";
}
