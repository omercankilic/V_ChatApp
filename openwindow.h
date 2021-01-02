#ifndef OPENWINDOW_H
#define OPENWINDOW_H

#include <QDialog>
#include <mainwindow.h>

namespace Ui {
        class OpenWindow;
}

class OpenWindow : public QDialog
{
                Q_OBJECT
                
        public:
                explicit OpenWindow(QWidget *parent = nullptr);
                ~OpenWindow();
                MainWindow *open_main_window;
                bool is_valid_ip(std::string ip_n);
        private slots:
                void on_loginButton_clicked();
                
        private:
                Ui::OpenWindow *ui;
                
};

#endif // OPENWINDOW_H
