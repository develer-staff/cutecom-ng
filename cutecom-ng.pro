#-------------------------------------------------
#
# Project created by QtCreator 2015-06-10T16:52:08
#
#-------------------------------------------------

QT       += core gui serialport uitools

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cutecom-ng
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    connectdialog.cpp \
    sessionmanager.cpp \
    outputmanager.cpp \
    historycombobox.cpp \
    history.cpp \
    searchhighlighter.cpp \
    xmodem.cpp \
    crc16.cpp \
    xmodemtransfer.cpp \
    filetransfer.cpp

HEADERS  += mainwindow.h \
    connectdialog.h \
    sessionmanager.h \
    outputmanager.h \
    historycombobox.h \
    history.h \
    searchhighlighter.h \
    xmodem.h \
    crc16.h \
    xmodemtransfer.h \
    filetransfer.h

FORMS    += mainwindow.ui \
    connectdialog.ui \
    searchwidget.ui

RESOURCES += \
    cutecom-ng.qrc
