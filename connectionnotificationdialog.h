#ifndef CONNECTIONNOTIFICATIONDIALOG_H
#define CONNECTIONNOTIFICATIONDIALOG_H

#include <QDialog>

namespace Ui {
class connectionNotificationDialog;
}

class connectionNotificationDialog : public QDialog
{
    Q_OBJECT

public:
    connectionNotificationDialog(QWidget *parent = nullptr, QString notification = "");
    ~connectionNotificationDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::connectionNotificationDialog *ui;
};

#endif // CONNECTIONNOTIFICATIONDIALOG_H
