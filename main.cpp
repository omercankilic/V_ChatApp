#include <openwindow.h>
#include <QApplication>

int main(int argc, char *argv[])
{
        QApplication a(argc, argv);
       
        OpenWindow open_window;
        open_window.show();
        //MainWindow w;
        //w.show();
        return a.exec();
        
        return 0;
}
