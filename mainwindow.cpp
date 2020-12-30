#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Tcp_Socket/TCP_SOCKET.h>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
{
        ui->setupUi(this);
}

MainWindow::~MainWindow()
{
        delete ui;
}


void MainWindow::on_removeClientButton_4_clicked()
{
        
}
