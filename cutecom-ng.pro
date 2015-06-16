#-------------------------------------------------
#
# Project created by QtCreator 2015-06-10T16:52:08
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cutecom-ng
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    connectdialog.cpp \
    sessionmanager.cpp \
    outputmanager.cpp \
    inputline.cpp

HEADERS  += mainwindow.h \
    connectdialog.h \
    sessionmanager.h \
    outputmanager.h \
    inputline.h

FORMS    += mainwindow.ui \
    connectdialog.ui
