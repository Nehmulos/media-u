#-------------------------------------------------
#
# Project created by QtCreator 2013-05-22T18:40:30
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MediaU
TEMPLATE = app

# qhttp
INCLUDEPATH += $$_PRO_FILE_PWD_/libs/qhttpserver/src
INCLUDEPATH += $$_PRO_FILE_PWD_/libs/qhttpserver
LIBPATH += $$_PRO_FILE_PWD_/libs/qhttpserver/lib
LIBS += -lqhttpserver


SOURCES += main.cpp\
        mainwindow.cpp \
    server.cpp

HEADERS  += mainwindow.h \
    server.h

FORMS    += mainwindow.ui
