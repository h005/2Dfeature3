#-------------------------------------------------
#
# Project created by QtCreator 2015-09-22T15:50:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 2Dfeature3
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    image.cpp

HEADERS  += mainwindow.h \
    image.h

FORMS    += mainwindow.ui

INCLUDEPATH += D:\tools\opencv\build\include\opencv\
                D:\tools\opencv\build\include\opencv2\
                D:\tools\opencv\build\include

LIBS += -LD:\tools\opencv\build\x64\vc12\lib \
        -lopencv_calib3d249d \
        -lopencv_contrib249d \
        -lopencv_core249d \
        -lopencv_features2d249d \
        -lopencv_flann249d \
        -lopencv_gpu249d \
        -lopencv_highgui249d \
        -lopencv_imgproc249d \
        -lopencv_legacy249d \
        -lopencv_ml249d \
        -lopencv_nonfree249d \
        -lopencv_objdetect249d \
        -lopencv_ocl249d \
        -lopencv_photo249d \
        -lopencv_stitching249d \
        -lopencv_superres249d \
        -lopencv_ts249d \
        -lopencv_video249d \
        -lopencv_videostab249d


