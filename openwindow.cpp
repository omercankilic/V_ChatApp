#include "openwindow.h"
#include "ui_openwindow.h"

OpenWindow::OpenWindow(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::OpenWindow)
{
        ui->setupUi(this);
        
}

OpenWindow::~OpenWindow()
{
        delete ui;
}

void OpenWindow::on_loginButton_clicked()
{
    if(ui->usernameLineEdit->text().isEmpty() == false){
            QString ip_name = ui->ipLineEdit->text();
            std::string ip_n = ip_name.toStdString();
            if(ip_name.length()!= 0){
                    if(is_valid_ip(ip_n)){
                            
                            QWidget *parent = nullptr;
                            std::string user_n = ui->usernameLineEdit->text().toStdString();
                            open_main_window = new MainWindow(parent,ip_n,user_n);
                            
                            open_main_window->show();
                            this->close();                            
                            
                    }
            }
    }
}
