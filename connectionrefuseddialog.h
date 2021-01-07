#ifndef CONNECTIONREFUSEDDIALOG_H
#define CONNECTIONREFUSEDDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectionRefusedDialog;
}

class ConnectionRefusedDialog : public QDialog
{
    Q_OBJECT

public:
    ConnectionRefusedDialog(QWidget *parent = nullptr, QString username="");
    ~ConnectionRefusedDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ConnectionRefusedDialog *ui;
};

#endif // CONNECTIONREFUSEDDIALOG_H
