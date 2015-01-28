#-------------------------------------------------
#
# Project created by QtCreator 2015-01-13T16:50:26
#
#-------------------------------------------------

QT       += core gui
QT       += xml

win32{
QMAKE_LFLAGS += -static -static-libgcc
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    animation.cpp \
    itemsfactory.cpp \
    vertex.cpp \
    edge.cpp \
    layer.cpp \
    animationframe.cpp

HEADERS  += mainwindow.h \
    animation.h \
    itemsfactory.h \
    vertex.h \
    edge.h \
    layer.h \
    animationframe.h

FORMS    += mainwindow.ui

CONFIG   += c++11
