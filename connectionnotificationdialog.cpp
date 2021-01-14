#include "connectionnotificationdialog.h"
#include "ui_connectionnotificationdialog.h"

connectionNotificationDialog::connectionNotificationDialog(QWidget *parent, QString notification) :
    QDialog(parent),
    ui(new Ui::connectionNotificationDialog)
{
    ui->setupUi(this);
    ui->label->setText(notification);
}

connectionNotificationDialog::~connectionNotificationDialog()
{
    delete ui;
}

void connectionNotificationDialog::on_pushButton_clicked()
{
    this->close();
}
