#ifndef CONNECTIONSTOPDIALOG_H
#define CONNECTIONSTOPDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectionStopDialog;
}

class ConnectionStopDialog : public QDialog
{
    Q_OBJECT

public:
    ConnectionStopDialog(QWidget *parent = nullptr, std::string username="");
    ~ConnectionStopDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ConnectionStopDialog *ui;
};

#endif // CONNECTIONSTOPDIALOG_H
