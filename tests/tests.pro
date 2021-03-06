#-------------------------------------------------
#
# Project created by QtCreator 2013-09-14T18:43:27
#
#-------------------------------------------------

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

# qhttp
INCLUDEPATH += $$_PRO_FILE_PWD_/../src/libs/qhttpserver/src
INCLUDEPATH += $$_PRO_FILE_PWD_/../src/libs/qhttpserver/http-parser
INCLUDEPATH += $$_PRO_FILE_PWD_/../src/libs/qhttpserver
LIBPATH += $$_PRO_FILE_PWD_/../src/libs/qhttpserver/lib
LIBS += -lqhttpserver

# libcurl
INCLUDEPATH += $$_PRO_FILE_PWD_/../src/libs/curl/include/
LIBPATH += $$_PRO_FILE_PWD_/../src/libs/curl/lib/.libs/
LIBS += -lcurl

# noslib
INCLUDEPATH += $$_PRO_FILE_PWD_/../src/libs/noslib/src/
INCLUDEPATH += $$_PRO_FILE_PWD_/../src/libs/noslib/
LIBPATH += $$_PRO_FILE_PWD_/../src/libs/noslib/
LIBS += -lN0Slib


TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += ../src


QT       += testlib

QT       -= gui

TARGET = tst_moviefiletest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += tst_moviefiletest.cpp \
    ../src/nwutils.cpp \
    ../src/episode.cpp \
    ../src/videofile.cpp \
    ../src/systemutils.cpp \
    ../src/utils.cpp

HEADERS  += ../src/nwutils.h \
    ../src/episode.h \
    ../src/videofile.h \
    ../src/systemutils.h \
    ../src/utils.h


DEFINES += SRCDIR=\\\"$$PWD/\\\"
