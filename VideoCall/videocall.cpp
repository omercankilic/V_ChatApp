#include "videocall.h"
#include "ui_videocall.h"

VideoCall::VideoCall(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoCall)
{
    ui->setupUi(this);
}

VideoCall::~VideoCall()
{
    delete ui;
}
