#-------------------------------------------------
#
# Project created by QtCreator 2017-08-06T20:01:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DKey
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    coupletDialog.cpp \
    dkCouplet.cpp \
    dkCoupletList.cpp \
    txtwindow.cpp \
    dkString.cpp

HEADERS  += mainwindow.h \
    coupletDialog.h \
    dkCouplet.h \
    dkCoupletList.h \
    txtwindow.h \
    dkString.h

OTHER_FILES += \
    DKey.rc \
    DKey_icon.ico

RC_FILE = DKey.rc

VERSION = 1.0.0
DEFINES += VERSION_STRING=\\\"1.0.0\\\"

