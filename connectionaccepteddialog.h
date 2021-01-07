#ifndef CONNECTIONACCEPTEDDIALOG_H
#define CONNECTIONACCEPTEDDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectionAcceptedDialog;
}

class ConnectionAcceptedDialog : public QDialog
{
    Q_OBJECT

public:
    ConnectionAcceptedDialog(QWidget *parent = nullptr, std::string username="");
    ~ConnectionAcceptedDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ConnectionAcceptedDialog *ui;
};

#endif // CONNECTIONACCEPTEDDIALOG_H
