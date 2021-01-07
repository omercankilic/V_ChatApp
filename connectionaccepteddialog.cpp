#include "connectionaccepteddialog.h"
#include "ui_connectionaccepteddialog.h"

ConnectionAcceptedDialog::ConnectionAcceptedDialog(QWidget *parent, QString username) :
    QDialog(parent),
    ui(new Ui::ConnectionAcceptedDialog)
{
    ui->setupUi(this);
    ui->label->setText(username + " accepted your connection request.");
}

ConnectionAcceptedDialog::~ConnectionAcceptedDialog()
{
    delete ui;
}

void ConnectionAcceptedDialog::on_pushButton_clicked()
{
    this->close();
}
