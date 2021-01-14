QT       += core gui multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets 

CONFIG += c++11


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Tcp_Socket/TCP_SOCKET.cpp \
    Udp_Socket/UDP_SOCKET.cpp \
    VideoCall/receiver.cpp \
    VideoCall/sender.cpp \
    VideoCall/video_funcs.cpp \
    VideoCall/videocall.cpp \
    VideoCall/videomessagepacket.cpp \
    camera_capture.cpp \
    connectionnotificationdialog.cpp \
    connectionstartdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    openwindow.cpp

HEADERS += \
    ActiveClients/Active_Clients.h \
    FFmpeg_Headers.h \
    Helper_Functions.h \
    MessagePacket.h \
    Tcp_Socket/TCP_SOCKET.h \
    Udp_Socket/UDP_SOCKET.h \
    VideoCall/receiver.h \
    VideoCall/sender.h \
    VideoCall/video_funcs.h \
    VideoCall/videocall.h \
    VideoCall/videomessagepacket.h \
    camera_capture.h \
    connectionnotificationdialog.h \
    connectionstartdialog.h \
    conversion_functions.h \
    mainwindow.h \
    openwindow.h 

FORMS += \
    connectionnotificationdialog.ui \
    connectionstartdialog.ui \
    mainwindow.ui \
    openwindow.ui \
    videocall.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


INCLUDEPATH  += /usr/include/opencv4
INCLUDEPATH += /home/mrcan/ffmpeg_build/include

LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_imgcodecs -lopencv_videoio

LIBS +=-L/usr/local/lib -lavdevice -lm -lxcb -lxcb-shm -lxcb -lxcb  -lxcb-render  -lxcb  -lavfilter -pthread -lm  -ldl -lswscale -lm -lavformat -lz -lavcodec -pthread -lm -llzma -lz  -ldl -lswresample -lm -lavutil -pthread  -lX11 -lm -ldl



