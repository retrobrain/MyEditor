#-------------------------------------------------
#
# Project created by QtCreator 2015-01-13T16:50:26
#
#-------------------------------------------------

QT       += core gui
QT       += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    animationframe.cpp \
    animation.cpp \
    itemsfactory.cpp \
    vertex.cpp \
    edge.cpp \

HEADERS  += mainwindow.h \
    animationframe.h \
    animation.h \
    itemsfactory.h \
    vertex.h \
    edge.h \

FORMS    += mainwindow.ui

CONFIG   += c++11
