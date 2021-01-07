#include "connectionrefuseddialog.h"
#include "ui_connectionrefuseddialog.h"

ConnectionRefusedDialog::ConnectionRefusedDialog(QWidget *parent, QString username) :
    QDialog(parent),
    ui(new Ui::ConnectionRefusedDialog)
{
    ui->setupUi(this);
    ui->label->setText(username + " refused your connection request.");
}

ConnectionRefusedDialog::~ConnectionRefusedDialog()
{
    delete ui;
}

void ConnectionRefusedDialog::on_pushButton_clicked()
{
    this->close();
}
