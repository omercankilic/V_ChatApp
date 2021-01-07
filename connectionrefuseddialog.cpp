#include "connectionrefuseddialog.h"
#include "ui_connectionrefuseddialog.h"

ConnectionRefusedDialog::ConnectionRefusedDialog(QWidget *parent, std::string username) :
    QDialog(parent),
    ui(new Ui::ConnectionRefusedDialog)
{
    ui->label->setText(QString::fromStdString(username) + "refused your connection request.");
    ui->setupUi(this);
}

ConnectionRefusedDialog::~ConnectionRefusedDialog()
{
    delete ui;
}

void ConnectionRefusedDialog::on_pushButton_clicked()
{
    this->close();
}
