#include "connectionstopdialog.h"
#include "ui_connectionstopdialog.h"

ConnectionStopDialog::ConnectionStopDialog(QWidget *parent, std::string username) :
    QDialog(parent),
    ui(new Ui::ConnectionStopDialog)
{
    ui->label->setText(QString::fromStdString(username) + "stopped connection.");
    ui->setupUi(this);
}

ConnectionStopDialog::~ConnectionStopDialog()
{
    delete ui;
}

void ConnectionStopDialog::on_pushButton_clicked()
{
    this->close();
}
