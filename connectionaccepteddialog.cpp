#include "connectionaccepteddialog.h"
#include "ui_connectionaccepteddialog.h"

ConnectionAcceptedDialog::ConnectionAcceptedDialog(QWidget *parent, std::string username) :
    QDialog(parent),
    ui(new Ui::ConnectionAcceptedDialog)
{
    ui->label->setText(QString::fromStdString(username) + "accepted your connection request.");
    ui->setupUi(this);
}

ConnectionAcceptedDialog::~ConnectionAcceptedDialog()
{
    delete ui;
}

void ConnectionAcceptedDialog::on_pushButton_clicked()
{
    this->close();
}
