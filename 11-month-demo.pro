QT       += core gui
QT       += axcontainer
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
    dataclass.cpp \
    delaytesting.cpp \
    ffmpegplayerclass.cpp \
    imagelabelclass.cpp \
    imageprocessingclass.cpp \
    inputgrayscaleresult.cpp \
    inputresolutionresult.cpp \
    inputyourname.cpp \
    main.cpp \
    mainwindow.cpp \
    makeexcle.cpp \
    playthread.cpp \
    qsliderclass.cpp \
    screenshotthread.cpp \
    subjecttesting.cpp

HEADERS += \
    dataclass.h \
    delaytesting.h \
    ffmpegplayerclass.h \
    imagelabelclass.h \
    imageprocessingclass.h \
    inputgrayscaleresult.h \
    inputresolutionresult.h \
    inputyourname.h \
    mainwindow.h \
    makeexcle.h \
    playthread.h \
    qsliderclass.h \
    screenshotthread.h \
    subjecttesting.h

FORMS += \
    delaytesting.ui \
    inputgrayscaleresult.ui \
    inputresolutionresult.ui \
    inputyourname.ui \
    mainwindow.ui \
    subjecttesting.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += E:\Opencv_source\opencv_QT\install\include \
               E:\Opencv_source\opencv_QT\install\include\opencv2

LIBS += E:\Opencv_source\opencv_QT\install\x64\mingw\bin\libopencv_core430.dll \
        E:\Opencv_source\opencv_QT\install\x64\mingw\bin\libopencv_highgui430.dll \
        E:\Opencv_source\opencv_QT\install\x64\mingw\bin\libopencv_imgproc430.dll \
        E:\Opencv_source\opencv_QT\install\x64\mingw\bin\libopencv_imgcodecs430.dll

INCLUDEPATH += F:/QTSpace/demo_testing/ffmpeg/include

#LIBS += F:/QTSpace/demo_testing/ffmpeg/lib/libavcodec.dll.a \
#        F:/QTSpace/demo_testing/ffmpeg/lib/libavformat.dll.a \
#        F:/QTSpace/demo_testing/ffmpeg/lib/libswscale.dll.a \
#        F:/QTSpace/demo_testing/ffmpeg/lib/libavutil.dll.a \
#        F:/QTSpace/demo_testing/ffmpeg/lib/libswresample.dll.a


LIBS += F:/QTSpace/demo_testing/ffmpeg/lib/avcodec.lib \
        F:/QTSpace/demo_testing/ffmpeg/lib/avformat.lib \
        F:/QTSpace/demo_testing/ffmpeg/lib/swscale.lib \
        F:/QTSpace/demo_testing/ffmpeg/lib/avutil.lib \
        F:/QTSpace/demo_testing/ffmpeg/lib/swresample.lib


