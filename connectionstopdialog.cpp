#include "connectionstopdialog.h"
#include "ui_connectionstopdialog.h"

ConnectionStopDialog::ConnectionStopDialog(QWidget *parent, QString username) :
    QDialog(parent),
    ui(new Ui::ConnectionStopDialog)
{
    ui->setupUi(this);
    ui->label->setText(username + " stopped connection.");
}

ConnectionStopDialog::~ConnectionStopDialog()
{
    delete ui;
}

void ConnectionStopDialog::on_pushButton_clicked()
{
    this->close();
}
