#include "openwindow.h"
#include "ui_openwindow.h"
#include <QFile>
#include <iostream>

OpenWindow::OpenWindow(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::OpenWindow)
{
        ui->setupUi(this);
        system("ifconfig | grep -Eo 'inet (addr:)?([0-9]*\\.){3}[0-9]*' | grep -Eo '([0-9]*\\.){3}[0-9]*' | grep -v '127.0.0.1' > my_ip.txt");
        QFile file("my_ip.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return;
        }

        ui->ipList->setEditable(true);
        ui->ipList->lineEdit()->setReadOnly(true);
        ui->ipList->lineEdit()->setAlignment(Qt::AlignLeft);
        while (!file.atEnd()) {
           QByteArray line = file.readLine();
           ui->ipList->addItem(line);
       }
        
}

OpenWindow::~OpenWindow()
{
        delete ui;
}


void OpenWindow::on_loginButton_clicked()
{
    if(ui->usernameLineEdit->text().isEmpty() == false){
            std::string ip_n   = ui->ipList->currentText().toStdString();
            std::string user_n = ui->usernameLineEdit->text().toStdString();

            QWidget *parent = nullptr;
            open_main_window = new MainWindow(parent,ip_n,user_n);
            open_main_window->show();
            this->close();

    }
}
