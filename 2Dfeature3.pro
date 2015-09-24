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

INCLUDEPATH += D:\opencv\include\opencv\
                D:\opencv\include\opencv2\
                D:\opencv\include\

LIBS += D:/opencv/lib/*.a


