#-------------------------------------------------
#
# Project created by QtCreator 2015-02-04T15:01:47
#
#-------------------------------------------------

QT       += core gui websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tt-server
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \ # since copied from echoservers
    wsserver.cpp

HEADERS  += mainwindow.h \
    wsserver.h

FORMS    += mainwindow.ui